//
//  KneeGraph.h
//  simpleMBCompressor
//
//  Created by Richard Kim on 7/3/25.
//  Copyright © 2025 Rich Kim. All rights reserved.
//

#pragma once

#include <JuceHeader.h>
#include "../DSP/CompressorBand.h" 

struct KneeGraph : juce::Component
{
    KneeGraph(CompressorBand& band);
    void paint(juce::Graphics& g) override;

private:
    CompressorBand& compressorBand;
};
