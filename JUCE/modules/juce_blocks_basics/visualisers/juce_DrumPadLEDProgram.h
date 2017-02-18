/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2016 - ROLI Ltd.

   Permission is granted to use this software under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license/

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS. IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
   OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
   OF THIS SOFTWARE.

   -----------------------------------------------------------------------------

   To release a closed-source product which uses other parts of JUCE not
   licensed under the ISC terms, commercial licenses are available: visit
   www.juce.com for more information.

  ==============================================================================
*/

/**
*/
struct DrumPadGridProgram  : public LEDGrid::Program
{
    DrumPadGridProgram (LEDGrid&);

    //==============================================================================
    /** These let the program dim pads which aren't having gestures performed on them. */

    void startTouch (float startX, float startY);
    void endTouch   (float startX, float startY);

    /** Creates trail effects similar to the onscreen pad trails. */
    void sendTouch (float x, float y, float z, LEDColour);

    //==============================================================================
    /** Call this to match animations to the project tempo.

        @param padIdx           The pad to update. 16 animated pads are supported, so 0 - 15.
        @param loopTimeSecs     The length of time for the pad's animation to loop in seconds. 0 will stop the animation.
        @param currentProgress  The starting progress of the animation. 0.0 - 1.0.
    */
    void setPadAnimationState (uint32 padIdx, double loopTimeSecs, double currentProgress);

    /** If the app needs to close down or suspend, use these to pause & dim animations. */
    void suspendAnimations();
    void resumeAnimations();

    //==============================================================================
    /** Set how each pad in the grid looks. */
    struct GridFill
    {
        enum FillType : uint8
        {
            gradient            = 0,
            filled              = 1,
            hollow              = 2,
            hollowPlus          = 3,

            // Animated pads
            dotPulsing          = 4,
            dotBlinking         = 5,
            pizzaFilled         = 6,
            pizzaHollow         = 7,
        };

        LEDColour colour;
        FillType fillType;
    };

    void setGridFills (int numColumns, int numRows,
                       const juce::Array<GridFill>&);

    /** Set up a new pad layout, with a slide animation from the old to the new. */
    enum SlideDirection : uint8
    {
        up     = 0,
        down   = 1,
        left   = 2,
        right  = 3,

        none   = 255
    };

    void triggerSlideTransition (int newNumColumns, int newNumRows,
                                 const juce::Array<GridFill>& newFills, SlideDirection);

private:
    //==============================================================================
    /** Shared data heap is laid out as below. There is room for two sets of
        pad layouts, colours and fill types to allow animation between two states. */

    static constexpr uint32 numColumns0_byte      = 0;    // 1 byte
    static constexpr uint32 numRows0_byte         = 1;    // 1 byte (ignored for the moment: always square pads to save cycles)
    static constexpr uint32 colours0_byte         = 2;    // 2 byte x 25  (5:6:5 bits for rgb)
    static constexpr uint32 fillTypes0_byte       = 52;   // 1 byte x 25

    static constexpr uint32 numColumns1_byte      = 78;   // 1 byte
    static constexpr uint32 numRows1_byte         = 79;   // 1 byte
    static constexpr uint32 colours1_byte         = 80;   // 2 byte x 25  (5:6:5 bits for rgb)
    static constexpr uint32 fillTypes1_byte       = 130;  // 1 byte x 25

    static constexpr uint32 visiblePads_byte      = 155;  // 1 byte       (i.e. which set of colours/fills to use, 0 or 1)
    static constexpr uint32 slideDirection_byte   = 156;  // 1 byte
    static constexpr uint32 touchedPads_byte      = 158;  // 1 byte x 4   (Zero means empty slot, so stores padIdx + 1)
    static constexpr uint32 animationTimers_byte  = 162;  // 4 byte x 16  (16:16 bits counter:increment)
    static constexpr uint32 heatMap_byte          = 226;  // 4 byte x 225
    static constexpr uint32 heatDecayMap_byte     = 1126; // 1 byte x 225

    static constexpr uint32 maxNumPads        = 25;
    static constexpr uint32 colourSizeBytes   = 2;
    static constexpr uint32 heatMapSize       = 15 * 15 * 4;
    static constexpr uint32 heatMapDecaySize  = 15 * 15;
    static constexpr uint32 totalDataSize     = heatDecayMap_byte + heatMapDecaySize;

    int getPadIndex (float posX, float posY) const;
    void setGridFills (int numColumns, int numRows, const juce::Array<GridFill>& fills, uint32 byteOffset);

    juce::String getLittleFootProgram() override;
    uint32 getHeapSize() override;
};
