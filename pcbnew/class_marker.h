/***************************************/
/* Markers: used to show a drc problem */
/***************************************/

#ifndef CLASS_MARKER_H
#define CLASS_MARKER_H

#include "base_struct.h"

#include "drc_stuff.h"

class MARKER : public BOARD_ITEM, public MARKER_BASE
{

public:

    MARKER( BOARD_ITEM* aParent );

    /**
     * Constructor
     * @param aErrorCode The categorizing identifier for an error
     * @param aMarkerPos The position of the MARKER on the BOARD
     * @param aText Text describing the first of two objects
     * @param aPos The position of the first of two objects
     * @param bText Text describing the second of the two conflicting objects
     * @param bPos The position of the second of two objects
     */
    MARKER( int aErrorCode, const wxPoint& aMarkerPos,
           const wxString& aText, const wxPoint& aPos,
           const wxString& bText, const wxPoint& bPos );
     /**
     * Constructor
     * @param aErrorCode The categorizing identifier for an error
     * @param aMarkerPos The position of the MARKER on the BOARD
     * @param aText Text describing the object
     * @param aPos The position of the object
     */
    MARKER( int aErrorCode, const wxPoint& aMarkerPos,
           const wxString& aText, const wxPoint& aPos );


    ~MARKER();

    void    UnLink();       // Deprecated


    /** Function Draw
     */
    void    Draw( WinEDA_DrawPanel* aPanel, wxDC* aDC, int aDrawMode, const wxPoint& aOffset = ZeroOffset )
    {
        DrawMarker( aPanel, aDC, aDrawMode, aOffset );
    }

    /**
     * Function GetPosition
     * returns the position of this MARKER.
     */
    wxPoint& GetPosition()
    {
        return (wxPoint&) m_Pos;
    }


    /** Function HitTest
     * @return true if the point aPosRef is within item area
     * @param aPosRef = a wxPoint to test
     */
    bool HitTest( const wxPoint& aPosRef )
    {
        return HitTestMarker( aPosRef );
    }

    /**
     * Function DisplayInfo
     * has knowledge about the frame and how and where to put status information
     * about this object into the frame's message panel.
     * @param frame A WinEDA_DrawFrame in which to print status information.
     */
    void    DisplayInfo( WinEDA_DrawFrame* frame );


    /**
     * Function Save
     * writes the data structures for this object out to a FILE in "*.brd" format.
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */
    bool Save( FILE* aFile ) const
    {
        // not implemented, this is here to satisfy BOARD_ITEM::Save()
        // "pure" virtual-ness
        return true;
    }
};


#endif      //  CLASS_MARKER_H
