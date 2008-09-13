/*****************/
/* class_pin.cpp */
/*****************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "general.h"
#include "protos.h"


/**********************************************************************************************/
void LibDrawPin::Draw( WinEDA_DrawPanel* aPanel, wxDC* aDC, const wxPoint& aOffset, int aColor,
                       int aDrawMode, void* aData, int aTransformMatrix[2][2] )
/**********************************************************************************************/
{
    if( ( m_Attributs & PINNOTDRAW ) && !g_ShowAllPins )
        return;

    EDA_LibComponentStruct* Entry = ( (DrawPinPrms*) aData )->m_Entry;
    bool    DrawPinText = ( (DrawPinPrms*) aData )->m_DrawPinText;

    /* Calculate Pin orient takin in account the component orientation */
    int     orient = ReturnPinDrawOrient( aTransformMatrix );

    /* Calculate the pin position */
    wxPoint pos1 = TransformCoordinate( aTransformMatrix, m_Pos ) + aOffset;

    /* Dessin de la pin et du symbole special associe */
    DrawPinSymbol( aPanel, aDC, pos1, orient, aDrawMode, aColor );

    if( DrawPinText )
    {
        DrawPinTexts( aPanel, aDC, pos1, orient,
            Entry->m_TextInside,
            Entry->m_DrawPinNum, Entry->m_DrawPinName,
            aColor, aDrawMode );
    }
}


/********************************************************************************/
void LibDrawPin::DrawPinSymbol( WinEDA_DrawPanel* aPanel, wxDC* aDC,
                                const wxPoint& aPinPos, int aOrient, int aDrawMode, int aColor )
/*******************************************************************************/

/* Draw the pin symbol (without texts)
 *  if Color != 0 draw with Color, else with the normal pin color
 */
{
    int MapX1, MapY1, x1, y1;
    int color;
    int width = MAX( m_Width, g_DrawMinimunLineWidth );
    int posX  = aPinPos.x, posY = aPinPos.y, len = m_PinLen;


    color = ReturnLayerColor( LAYER_PIN );
    if( aColor < 0 )       // Used normal color or selected color
    {
        if( (m_Selected & IS_SELECTED) )
            color = g_ItemSelectetColor;
    }
    else
        color = aColor;

    GRSetDrawMode( aDC, aDrawMode );

    MapX1 = MapY1 = 0; x1 = posX; y1 = posY;

    switch( aOrient )
    {
    case PIN_UP:
        y1 = posY - len; MapY1 = 1;
        break;

    case PIN_DOWN:
        y1 = posY + len; MapY1 = -1;
        break;

    case PIN_LEFT:
        x1 = posX - len, MapX1 = 1;
        break;

    case PIN_RIGHT:
        x1 = posX + len; MapX1 = -1;
        break;
    }

    if( m_PinShape & INVERT )
    {
        GRCircle( &aPanel->m_ClipBox, aDC, MapX1 * INVERT_PIN_RADIUS + x1,
            MapY1 * INVERT_PIN_RADIUS + y1,
            INVERT_PIN_RADIUS, width, color );

        GRMoveTo( MapX1 * INVERT_PIN_RADIUS * 2 + x1,
            MapY1 * INVERT_PIN_RADIUS * 2 + y1 );
        GRLineTo( &aPanel->m_ClipBox, aDC, posX, posY, width, color );
    }
    else
    {
        GRMoveTo( x1, y1 );
        GRLineTo( &aPanel->m_ClipBox, aDC, posX, posY, width, color );
    }

    if( m_PinShape & CLOCK )
    {
        if( MapY1 == 0 ) /* MapX1 = +- 1 */
        {
            GRMoveTo( x1, y1 + CLOCK_PIN_DIM );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1 - MapX1 * CLOCK_PIN_DIM, y1, width, color );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1, y1 - CLOCK_PIN_DIM, width, color );
        }
        else    /* MapX1 = 0 */
        {
            GRMoveTo( x1 + CLOCK_PIN_DIM, y1 );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1, y1 - MapY1 * CLOCK_PIN_DIM, width, color );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1 - CLOCK_PIN_DIM, y1, width, color );
        }
    }

    if( m_PinShape & LOWLEVEL_IN )  /* IEEE symbol "Active Low Input" */
    {
        if( MapY1 == 0 )            /* MapX1 = +- 1 */
        {
            GRMoveTo( x1 + MapX1 * IEEE_SYMBOL_PIN_DIM * 2, y1 );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1 + MapX1 * IEEE_SYMBOL_PIN_DIM * 2,
                y1 - IEEE_SYMBOL_PIN_DIM, width, color );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1, y1, width, color );
        }
        else    /* MapX1 = 0 */
        {
            GRMoveTo( x1, y1 + MapY1 * IEEE_SYMBOL_PIN_DIM * 2 );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1 - IEEE_SYMBOL_PIN_DIM,
                y1 + MapY1 * IEEE_SYMBOL_PIN_DIM * 2, width, color );
            GRLineTo( &aPanel->m_ClipBox, aDC, x1, y1, width, color );
        }
    }


    if( m_PinShape & LOWLEVEL_OUT ) /* IEEE symbol "Active Low Output" */
    {
        if( MapY1 == 0 )            /* MapX1 = +- 1 */
        {
            GRMoveTo( x1, y1 - IEEE_SYMBOL_PIN_DIM );
            GRLineTo( &aPanel->m_ClipBox,
                aDC,
                x1 + MapX1 * IEEE_SYMBOL_PIN_DIM * 2,
                y1,
                width,
                color );
        }
        else    /* MapX1 = 0 */
        {
            GRMoveTo( x1 - IEEE_SYMBOL_PIN_DIM, y1 );
            GRLineTo( &aPanel->m_ClipBox,
                aDC,
                x1,
                y1 + MapY1 * IEEE_SYMBOL_PIN_DIM * 2,
                width,
                color );
        }
    }

    /* Draw the pin end target (active end of the pin) */
    if( !g_IsPrinting )  // Draw but do not print the pin end target 1 pixel width */
        GRCircle( &aPanel->m_ClipBox, aDC, posX, posY, TARGET_PIN_DIAM, 0, color );
}


/*****************************************************************************
*  Put out pin number and pin text info, given the pin line coordinates.
*  The line must be vertical or horizontal.
*  If PinText == NULL nothing is printed. If PinNum = 0 no number is printed.
*  Current Zoom factor is taken into account.
*  If TextInside then the text is been put inside,otherwise all is drawn outside.
*  Pin Name:	substring beteween '~' is negated
*****************************************************************************/
void LibDrawPin::DrawPinTexts( WinEDA_DrawPanel* panel, wxDC* DC,
                               wxPoint& pin_pos, int orient,
                               int TextInside, bool DrawPinNum, bool DrawPinName,
                               int Color, int DrawMode )
/* DrawMode = GR_OR, XOR ... */
{
    int      ii, x, y, x1, y1, dx, dy, len;
    wxString StringPinNum;
    wxString PinText;
    int      PinTextBarPos[256];
    int      PinTextBarCount;
    int      NameColor, NumColor;
    int      PinTxtLen;

    wxSize   PinNameSize( m_PinNameSize, m_PinNameSize );
    wxSize   PinNumSize( m_PinNumSize, m_PinNumSize );

    int      LineWidth = g_DrawMinimunLineWidth;

    GRSetDrawMode( DC, DrawMode );

    /* Get the num and name colors */
    if( (Color < 0) && (m_Selected & IS_SELECTED) )
        Color = g_ItemSelectetColor;
    NameColor = Color == -1 ? ReturnLayerColor( LAYER_PINNAM ) : Color;
    NumColor  = Color == -1 ? ReturnLayerColor( LAYER_PINNUM ) : Color;

    /* Create the pin num string */
    ReturnPinStringNum( StringPinNum );

    x1 = pin_pos.x; y1 = pin_pos.y;

    switch( orient )
    {
    case PIN_UP:
        y1 -= m_PinLen; break;

    case PIN_DOWN:
        y1 += m_PinLen; break;

    case PIN_LEFT:
        x1 -= m_PinLen; break;

    case PIN_RIGHT:
        x1 += m_PinLen; break;
    }

    const wxChar* textsrc = m_PinName.GetData();
    float         fPinTextPitch = PinNameSize.x * 1.1;
    /* Do we need to invert the string? Is this string has only "~"? */
    PinTextBarCount = 0; PinTxtLen = 0;
    ii = 0;
    while( *textsrc )
    {
        if( *textsrc == '~' )
        {
            PinTextBarPos[PinTextBarCount++] = (int) (PinTxtLen * fPinTextPitch);
        }
        else
        {
            PinText.Append( *textsrc );
            PinTxtLen++;
        }

        textsrc++;
    }

    PinTxtLen = (int) (fPinTextPitch * PinTxtLen);
    PinTextBarPos[PinTextBarCount] = PinTxtLen; // Needed if no end '~'

    if( PinText[0] == 0 )
        DrawPinName = FALSE;

    if( TextInside )  /* Draw the text inside, but the pin numbers outside. */
    {
        if( (orient == PIN_LEFT) || (orient == PIN_RIGHT) )

        // It is an horizontal line
        {
            if( PinText && DrawPinName )
            {
                if( orient == PIN_RIGHT )
                {
                    x = x1 + TextInside;
                    DrawGraphicText( panel, DC, wxPoint( x, y1 ), NameColor, PinText,
                        TEXT_ORIENT_HORIZ,
                        PinNameSize,
                        GR_TEXT_HJUSTIFY_LEFT, GR_TEXT_VJUSTIFY_CENTER, LineWidth );

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        GRMoveTo( x, y1 - TXTMARGE );
                        dy = -PinNameSize.y / 2;
                        GRMoveRel( 0, dy );
                        dx = PinTextBarPos[ii++];       // Get the line pos
                        GRMoveRel( dx, 0 );
                        len = PinTextBarPos[ii++] - dx; // Get the line length
                        GRLineRel( &panel->m_ClipBox, DC, len, 0, LineWidth, NameColor );
                    }
                }
                else    // Orient == PIN_LEFT
                {
                    x = x1 - TextInside;
                    DrawGraphicText( panel, DC, wxPoint( x, y1 ), NameColor, PinText,
                        TEXT_ORIENT_HORIZ,
                        PinNameSize,
                        GR_TEXT_HJUSTIFY_RIGHT, GR_TEXT_VJUSTIFY_CENTER, LineWidth );

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        GRMoveTo( x, y1 - TXTMARGE );
                        dy = -PinNameSize.y / 2;
                        GRMoveRel( 0, dy );
                        dx = PinTextBarPos[ii++];       // Get the line pos
                        GRMoveRel( dx - PinTxtLen, 0 );
                        len = PinTextBarPos[ii++] - dx; // Get the line length
                        GRLineRel( &panel->m_ClipBox, DC, len, 0, LineWidth, NameColor );
                    }
                }
            }

            if( DrawPinNum )
            {
                DrawGraphicText( panel, DC,
                    wxPoint( (x1 + pin_pos.x) / 2, y1 - TXTMARGE ), NumColor, StringPinNum,
                    TEXT_ORIENT_HORIZ, PinNumSize,
                    GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_BOTTOM, LineWidth );
            }
        }
        else            /* Its a vertical line. */
        {
            // Text is drawn from bottom to top (i.e. to negative value for Y axis)
            if( PinText && DrawPinName )
            {
                if( orient == PIN_DOWN )
                {
                    y = y1 + TextInside;

                    DrawGraphicText( panel, DC, wxPoint( x1, y ), NameColor, PinText,
                        TEXT_ORIENT_VERT, PinNameSize,
                        GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_TOP, LineWidth );

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        GRMoveTo( x1 - TXTMARGE, y );
                        dy = -PinNameSize.y / 2;
                        GRMoveRel( dy, 0 );
                        dx = PinTextBarPos[ii++];       // Get the line pos
                        GRMoveRel( 0, PinTxtLen - dx );
                        len = PinTextBarPos[ii++] - dx; // Get the line length
                        GRLineRel( &panel->m_ClipBox, DC, 0, -len, LineWidth, NameColor );
                    }
                }
                else    /* PIN_UP */
                {
                    y = y1 - TextInside;

                    DrawGraphicText( panel, DC, wxPoint( x1, y ), NameColor, PinText,
                        TEXT_ORIENT_VERT, PinNameSize,
                        GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_BOTTOM, LineWidth );

                    for( ii = 0; ii < PinTextBarCount; )
                    {
                        GRMoveTo( x1 - TXTMARGE, y );
                        dy = -PinNameSize.y / 2;
                        GRMoveRel( dy, 0 );
                        dx = PinTextBarPos[ii++];       // Get the line pos
                        GRMoveRel( 0, -dx );
                        len = PinTextBarPos[ii++] - dx; // Get the line length
                        GRLineRel( &panel->m_ClipBox, DC, 0, -len, LineWidth, NameColor );
                    }
                }
            }

            if( DrawPinNum )
            {
                DrawGraphicText( panel, DC,
                    wxPoint( x1 - TXTMARGE, (y1 + pin_pos.y) / 2 ), NumColor, StringPinNum,
                    TEXT_ORIENT_VERT, PinNumSize,
                    GR_TEXT_HJUSTIFY_RIGHT, GR_TEXT_VJUSTIFY_CENTER, LineWidth );
            }
        }
    }
    else     /**** Draw num & text pin outside  ****/
    {
        if( (orient == PIN_LEFT) || (orient == PIN_RIGHT) )
        /* Its an horizontal line. */
        {
            if( PinText && DrawPinName )
            {
                x = (x1 + pin_pos.x) / 2;
                DrawGraphicText( panel, DC, wxPoint( x, y1 - TXTMARGE ),
                    NameColor, PinText,
                    TEXT_ORIENT_HORIZ, PinNameSize,
                    GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_BOTTOM, LineWidth );

                for( ii = 0; ii < PinTextBarCount; )
                {
                    GRMoveTo( x, y1 - TXTMARGE * 2 );
                    GRMoveRel( -PinTxtLen / 2, -PinNameSize.y );
                    dx = PinTextBarPos[ii++];       // Get the line pos
                    GRMoveRel( dx, 0 );
                    len = PinTextBarPos[ii++] - dx; // Get the line length
                    GRLineRel( &panel->m_ClipBox, DC, len, 0, LineWidth, NameColor );
                }
            }
            if( DrawPinNum )
            {
                x = (x1 + pin_pos.x) / 2;
                DrawGraphicText( panel, DC, wxPoint( x, y1 + TXTMARGE ),
                    NumColor, StringPinNum,
                    TEXT_ORIENT_HORIZ, PinNumSize,
                    GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_TOP, LineWidth );
            }
        }
        else     /* Its a vertical line. */
        {
            if( PinText && DrawPinName )
            {
                y = (y1 + pin_pos.y) / 2;
                DrawGraphicText( panel, DC, wxPoint( x1 - TXTMARGE, y ),
                    NameColor, PinText,
                    TEXT_ORIENT_VERT, PinNameSize,
                    GR_TEXT_HJUSTIFY_RIGHT, GR_TEXT_VJUSTIFY_CENTER, LineWidth );

                for( ii = 0; ii < PinTextBarCount; )
                {
                    GRMoveTo( x1 - (TXTMARGE * 2), y );
                    GRMoveRel( -PinNameSize.y, -PinTxtLen / 2 );
                    dx = PinTextBarPos[ii++];       // Get the line pos
                    GRMoveRel( 0, PinTxtLen - dx );
                    len = PinTextBarPos[ii++] - dx; // Get the line length
                    GRLineRel( &panel->m_ClipBox, DC, 0, -len, LineWidth, NameColor );
                }
            }

            if( DrawPinNum )
            {
                DrawGraphicText( panel, DC, wxPoint( x1 + TXTMARGE, (y1 + pin_pos.y) / 2 ),
                    NumColor, StringPinNum,
                    TEXT_ORIENT_VERT, PinNumSize,
                    GR_TEXT_HJUSTIFY_LEFT, GR_TEXT_VJUSTIFY_CENTER, LineWidth );
            }
        }
    }
}
