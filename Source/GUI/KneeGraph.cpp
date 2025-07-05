//
//  KneeGraph.cpp
//  simpleMBCompressor
//
//  Created by Richard Kim on 7/4/25.
//  Copyright © 2025 Rich Kim. All rights reserved.
//

#include "KneeGraph.h"
#include "LookAndFeel.h"
#include "Utilities.h"

using namespace juce;

KneeGraph::KneeGraph()
{
    startTimerHz(30); // 30 FPS update rate
}

KneeGraph::~KneeGraph()
{
    stopTimer();
}

void KneeGraph::paint(juce::Graphics& g)
{
    
    auto bounds = getLocalBounds().toFloat();
    auto graphBounds = bounds.reduced(20.0f); // Leave margin for labels
    
    // Clear background
//    g.fillAll(ColorScheme::getModuleBackgroundColor());
    g.fillAll(ColorScheme::getTitleColor());
    
    // Draw border
    g.setColour(ColorScheme::getModuleBorderColor());
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
    
    // Draw grid
    drawGrid(g, graphBounds);
    
    // Draw compression curve
    drawCurve(g, graphBounds);
    
    // Draw current input level indicator
    drawInputLevel(g, graphBounds);
    
    // Draw labels
    drawLabels(g, bounds);
}

void KneeGraph::resized()
{
    // Component resizing logic if needed
}

void KneeGraph::timerCallback()
{
    // Repaint to update the input level indicator
    repaint();
}

void KneeGraph::updateParams(float threshold, float ratio, float knee)
{
    thresholdDb = threshold;
    this->ratio = jmax(1.0f, ratio);
    kneeWidth = jlimit(0.0f, 20.0f, knee);
    repaint();
}

void KneeGraph::setInputLevel(float inputLevelDb)
{
    currentInputLevel = juce::jlimit(minDb - 10.0f, maxDb + 10.0f, inputLevelDb);
}

void KneeGraph::drawGrid(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    
    if (bounds.getWidth() <= 0 || bounds.getHeight() <= 0)
        return;
    
    g.setColour(ColorScheme::getModuleBorderColor().withAlpha(0.3f));
    
    // Vertical grid lines (input levels)
    for (float db = minDb; db <= maxDb; db += minorGridDb)
    {
        float x = jmap(db, minDb, maxDb, bounds.getX(), bounds.getRight());
        
        if (x >= bounds.getX() && x <= bounds.getRight())
        {
            if (fmod(db, majorGridDb) == 0.0f)
            {
                g.setColour(ColorScheme::getModuleBorderColor().withAlpha(0.5f));
                g.drawVerticalLine(roundToInt(x), bounds.getY(), bounds.getBottom());
            }
            else
            {
                g.setColour(ColorScheme::getModuleBorderColor().withAlpha(0.2f));
                g.drawVerticalLine(roundToInt(x), bounds.getY(), bounds.getBottom());
            }
        }
    }
    
    // Horizontal grid lines (output levels)
    for (float db = minDb; db <= maxDb; db += minorGridDb)
    {
        float y = jmap(db, maxDb, minDb, bounds.getY(), bounds.getBottom());
        
        if (y >= bounds.getY() && y <= bounds.getBottom())
        {
            if (fmod(db, majorGridDb) == 0.0f)
            {
                g.setColour(ColorScheme::getModuleBorderColor().withAlpha(0.5f));
                g.drawHorizontalLine(roundToInt(y), bounds.getX(), bounds.getRight());
            }
            else
            {
                g.setColour(ColorScheme::getModuleBorderColor().withAlpha(0.2f));
                g.drawHorizontalLine(roundToInt(y), bounds.getX(), bounds.getRight());
            }
        }
    }
    
    // Draw diagonal unity line
    g.setColour(ColorScheme::getModuleBorderColor().withAlpha(0.4f));
    g.drawLine(bounds.getX(), bounds.getBottom(), bounds.getRight(), bounds.getY(), 1.0f);
}

void KneeGraph::drawCurve(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    
    if (bounds.getWidth() <= 0 || bounds.getHeight() <= 0)
        return;
    
    Path curve;
    bool firstPoint = true;
    
    // Create the compression curve
    for (float inputDb = minDb; inputDb <= maxDb; inputDb += 0.5f)
    {
        float outputDb = applyCompression(inputDb);
        auto point = dbToPoint(inputDb, outputDb, bounds);
        
        if (std::isfinite(point.x) && std::isfinite(point.y))
        {
            if (firstPoint)
            {
                curve.startNewSubPath(point);
                firstPoint = false;
            }
            else
            {
                curve.lineTo(point);
            }
        }
    }
    
    // Draw the curve
    g.setColour(ColorScheme::getSliderFillColor());
    g.strokePath(curve, PathStrokeType(2.0f));
    
    // Draw threshold line
    g.setColour(ColorScheme::getSliderBorderColor().withAlpha(0.7f));
    float thresholdX = jmap(thresholdDb, minDb, maxDb, bounds.getX(), bounds.getRight());
    g.drawVerticalLine(roundToInt(thresholdX), bounds.getY(), bounds.getBottom());
    
    // Draw threshold label
    g.setColour(ColorScheme::getSliderBorderColor());
    g.setFont(10.0f);
    g.drawText(String(thresholdDb, 1) + " dB",
               Rectangle<float>(thresholdX - 20, bounds.getY() - 15, 40, 12),
               Justification::centred);
}

void KneeGraph::drawInputLevel(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    
    if (currentInputLevel >= minDb && currentInputLevel <= maxDb)
    {
        float inputX = jmap(currentInputLevel, minDb, maxDb, bounds.getX(), bounds.getRight());
        float outputDb = applyCompression(currentInputLevel);
        auto point = dbToPoint(currentInputLevel, outputDb, bounds);
        
        // Draw vertical line for input level
        g.setColour(Colours::yellow.withAlpha(0.8f));
        g.drawVerticalLine(roundToInt(inputX), bounds.getY(), bounds.getBottom());
        
        // Draw point on curve
        g.setColour(Colours::yellow);
        g.fillEllipse(point.x - 3, point.y - 3, 6, 6);
        
        // Draw reduction amount
        float reductionDb = currentInputLevel - outputDb;
        if (reductionDb > 0.1f)
        {
            g.setColour(Colours::red.withAlpha(0.7f));
            g.setFont(9.0f);
            g.drawText(String(-reductionDb, 1) + " dB",
                       Rectangle<float>(point.x - 25, point.y - 20, 50, 12),
                       Justification::centred);
        }
    }
}

void KneeGraph::drawLabels(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    
    g.setColour(ColorScheme::getSliderBorderColor());
    g.setFont(10.0f);
    
    // Input axis label
    g.drawText("Input (dB)",
               Rectangle<float>(bounds.getX(), bounds.getBottom() - 15, bounds.getWidth(), 12),
               Justification::centred);
    
    // Output axis label (rotated)
    g.saveState();
    g.addTransform(AffineTransform::rotation(-MathConstants<float>::halfPi,
                                           bounds.getX() + 10, bounds.getCentreY()));
    g.drawText("Output (dB)",
               Rectangle<float>(bounds.getX() + 10 - 30, bounds.getCentreY() - 6, 60, 12),
               Justification::centred);
    g.restoreState();
    
    // Ratio label
    g.setFont(12.0f);
    g.drawText("Ratio: " + String(ratio, 1) + ":1",
               Rectangle<float>(bounds.getRight() - 80, bounds.getY() + 5, 75, 15),
               Justification::centredRight);
}

juce::Point<float> KneeGraph::dbToPoint(float inputDb, float outputDb, juce::Rectangle<float> bounds)
{
    
    float x = jmap(inputDb, minDb, maxDb, bounds.getX(), bounds.getRight());
    float y = jmap(outputDb, maxDb, minDb, bounds.getY(), bounds.getBottom());
    
    return {x, y};
}

float KneeGraph::applyCompression(float inputDb)
{
    if (inputDb <= thresholdDb)
    {
        // Below threshold - no compression
        return inputDb;
    }
    else
    {
        // Above threshold - apply compression
        float overThreshold = inputDb - thresholdDb;
        float compressedOver = overThreshold / ratio;
        return thresholdDb + compressedOver;
    }
}

juce::Path KneeGraph::createCompressionCurve()
{
    
    Path curve;
    bool firstPoint = true;
    
    for (float inputDb = minDb; inputDb <= maxDb; inputDb += 0.5f)
    {
        float outputDb = applyCompression(inputDb);
        
        if (firstPoint)
        {
            curve.startNewSubPath(inputDb, outputDb);
            firstPoint = false;
        }
        else
        {
            curve.lineTo(inputDb, outputDb);
        }
    }
    
    return curve;
}
