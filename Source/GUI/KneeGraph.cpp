//
//  KneeGraph.cpp
//  simpleMBCompressor
//
//  Created by Richard Kim on 7/4/25.
//  Copyright © 2025 Rich Kim. All rights reserved.
//

#include "KneeGraph.h"

KneeGraph::KneeGraph(CompressorBand& band) : compressorBand(band) {}

void KneeGraph::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colours::white);
    g.fillRect(bounds);

    g.setColour(juce::Colours::blue);

    auto threshold = compressorBand.threshold->get();
    auto ratio = compressorBand.ratio->getCurrentChoiceName().getFloatValue();
    auto kneeWidth = 6.0f; // Example knee width

    juce::Path kneePath;
    for (float input = -60.0f; input <= 0.0f; input += 0.1f)
    {
        float output = input;
        if (input > threshold - kneeWidth / 2.0f)
        {
            if (input < threshold + kneeWidth / 2.0f)
            {
                float kneeStart = threshold - kneeWidth / 2.0f;
                float kneeEnd = threshold + kneeWidth / 2.0f;
                float kneeRatio = (input - kneeStart) / (kneeEnd - kneeStart);
                output = threshold + kneeRatio * (input - threshold) / ratio;
            }
            else
            {
                output = threshold + (input - threshold) / ratio;
            }
        }
        kneePath.lineTo(bounds.getWidth() * (input + 60.0f) / 60.0f, bounds.getHeight() * (1.0f - (output + 60.0f) / 60.0f));
    }

    g.strokePath(kneePath, juce::PathStrokeType(2.0f));
}
