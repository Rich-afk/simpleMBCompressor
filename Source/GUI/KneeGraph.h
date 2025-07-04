//
//  KneeGraph.h
//  simpleMBCompressor
//
//  Created by Richard Kim on 7/3/25.
//  Copyright © 2025 Rich Kim. All rights reserved.
//

#pragma once

#include <JuceHeader.h>

struct KneeGraph : juce::Component, juce::Timer
{
    KneeGraph();
    ~KneeGraph() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
    void updateParams(float threshold, float ratio, float knee = 0.0f);
    void setInputLevel(float inputLevelDb);
    
private:
    float thresholdDb = -12.0f;
    float ratio = 4.0f;
    float kneeWidth = 0.0f;
    float currentInputLevel = -60.0f;
    
    // Display range
    static constexpr float minDb = -60.0f;
    static constexpr float maxDb = 12.0f;
    
    // Grid settings
    static constexpr float majorGridDb = 12.0f;
    static constexpr float minorGridDb = 6.0f;
    
    juce::Path createCompressionCurve();
    juce::Point<float> dbToPoint(float inputDb, float outputDb, juce::Rectangle<float> bounds);
    float applyCompression(float inputDb);
    void drawGrid(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawCurve(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawInputLevel(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawLabels(juce::Graphics& g, juce::Rectangle<float> bounds);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KneeGraph)
};
