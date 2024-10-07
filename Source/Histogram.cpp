/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2022 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Histogram.h"

Histogram::Histogram (uint16 streamId_)
    : streamId (streamId_)
{
    hoverLabel = std::make_unique<Label> ("hover label");
    hoverLabel->setJustificationType (Justification::topLeft);
    hoverLabel->setFont (11);
    hoverLabel->addMouseListener (this, true);
    addAndMakeVisible (hoverLabel.get());

    statsLabel = std::make_unique<Label> ("stats label");
    statsLabel->setJustificationType (Justification::topLeft);
    statsLabel->setFont (11);
    statsLabel->addMouseListener (this, true);
    addAndMakeVisible (statsLabel.get());

    clear();
}

void Histogram::resized()
{
    histogramWidth = getWidth() - 2;
    histogramHeight = getHeight() - 2;

    statsLabel->setBounds (5, 5, 100, 45);
    hoverLabel->setBounds (histogramWidth - 85, histogramHeight - 33, 85, 25);
}

void Histogram::clear()
{
    pre_ms = 0;
    post_ms = 10;
    setBinSizeMs (1);

    relativeTimes.clear();

    numTrials = 0;

    refreshNeeded = true;

    recount();
}

void Histogram::addLatency (double latency)
{
    relativeTimes.add (latency);

    if (latency > post_ms)
        setWindowSizeMs (0, latency + 10);

    numTrials++;

    refreshNeeded = true;
}

void Histogram::setWindowSizeMs (int pre, int post)
{
    pre_ms = pre;
    post_ms = post;

    if (post_ms < 10)
        setBinSizeMs (1);
    else if (post_ms >= 10 && post_ms < 100)
        setBinSizeMs (5);
    else if (post_ms >= 100 && post_ms < 250)
        setBinSizeMs (10);
    else if (post_ms >= 250 && post_ms < 500)
        setBinSizeMs (25);
    else
        setBinSizeMs (50);

    recount();
}

void Histogram::setBinSizeMs (int ms)
{
    bin_size_ms = ms;

    binEdges.clear();

    double bin_edge = (double) -pre_ms;

    while (bin_edge < post_ms)
    {
        binEdges.add (bin_edge);
        bin_edge += (double) bin_size_ms;
    }

    binEdges.add (post_ms);

    recount();
}

void Histogram::recount (bool full)
{
    if (! refreshNeeded)
        return;

    const int nBins = binEdges.size() - 1;
    maxCount = 1;

    counts.clear();
    counts.insertMultiple (0, 0, nBins);

    float totalLatency = 0;

    for (int i = 0; i < relativeTimes.size(); i++)
    {
        totalLatency += relativeTimes[i];

        for (int j = 0; j < nBins; j++)
        {
            if ((relativeTimes[i] > binEdges[j]) && (relativeTimes[i] <= binEdges[j + 1]))
            {
                int lastValue = counts[j];

                counts.set (j, lastValue + 1);
                continue;
            }
        }
    }

    if (numTrials > 0)
    {
        mean = totalLatency / numTrials;

        std = 0.0f;

        for (int i = 0; i < relativeTimes.size(); i++)
        {
            std += pow (relativeTimes[i] - mean, 2);
        }

        std = sqrt (std / numTrials);

        for (int i = 0; i < counts.size(); i++)
        {
            if (counts[i] > maxCount)
                maxCount = counts[i];
        }

        statsLabel->setText ("samples: " + String (numTrials) + "\nmean: " + String (mean, 1, false) + " ms"
                                 + "\nstd: " + String (std, 1, false) + " ms",
                             dontSendNotification);
    }
    else
    {
        statsLabel->setText ("samples: " + String (numTrials) + "\nmean: --"
                                 + "\nstd: --",
                             dontSendNotification);
    }

    if (hoverBin >= 0 && hoverBin <= nBins)
    {
        String countString = "count: " + String (counts[hoverBin]);
        String binString = "[" + String (binEdges[hoverBin]) + "," + String (binEdges[hoverBin + 1]) + "] ms";

        hoverLabel->setText (countString + "\n" + binString, dontSendNotification);
    }

    repaint();

    refreshNeeded = false;
}

void Histogram::timerCallback()
{
    recount();
}

void Histogram::paint (Graphics& g)
{
    auto localBounds = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (findColour (ThemeColours::widgetBackground));
    g.fillRoundedRectangle (localBounds, 3.0f);

    const int nBins = binEdges.size() - 1;
    float binWidth = histogramWidth / float (nBins);

    Colour plotColour = findColour (ThemeColours::highlightedFill);

    for (int i = 0; i < nBins; i++)
    {
        if (hoverBin == i)
            g.setColour (plotColour.darker().withAlpha (0.85f));
        else
            g.setColour (plotColour);

        float x = binWidth * i;
        float relativeHeight = float (counts[i]) / float (maxCount);
        float height = relativeHeight * (histogramHeight - 10); // 10 pixels from top for padding
        float y = histogramHeight - height;
        g.fillRect (x, y, binWidth + 0.5f, height);
    }

    g.setColour (findColour (ThemeColours::outline));
    g.drawRoundedRectangle (localBounds, 3.0f, 1.0f);
}

void Histogram::mouseMove (const MouseEvent& event)
{
    MouseEvent relativeEvent = event.getEventRelativeTo (this);

    if (relativeEvent.getPosition().x < histogramWidth)
    {
        const int nBins = binEdges.size() - 1;
        float binWidth = histogramWidth / float (nBins);

        hoverBin = (int) (float (relativeEvent.getPosition().x) / binWidth);

        String countString = "count: " + String (counts[hoverBin]);
        String binString = "[" + String (binEdges[hoverBin]) + "," + String (binEdges[hoverBin + 1]) + "] ms";

        hoverLabel->setText (countString + "\n" + binString, dontSendNotification);

        repaint();
    }
}

void Histogram::mouseExit (const MouseEvent& event)
{
    hoverLabel->setText ("", dontSendNotification);
    hoverBin = -1;
    repaint();
}

void Histogram::mouseDown (const MouseEvent& event)
{
    if (event.mods.isRightButtonDown())
    {
        PopupMenu menu;
        menu.addItem (1, "Clear", true);
        menu.addItem (2, "Save values...", true);

        const int result = menu.show();

        switch (result)
        {
            case 1:
                clear();
                break;
            case 2:
                save();
                break;
        }
    }
}

void Histogram::save()
{
    DynamicObject output = getInfo();

    FileChooser chooser ("Save histogram statistics to file...",
                         File(),
                         "*.json");

    if (chooser.browseForFileToSave (true))
    {
        File file = chooser.getResult();

        if (file.exists())
            file.deleteFile();

        FileOutputStream f (file);

        output.writeAsJSON (f, JSON::FormatOptions {}.withIndentLevel (4).withSpacing (JSON::Spacing::multiLine).withMaxDecimalPlaces (4));
    }
}

DynamicObject Histogram::getInfo()
{
    DynamicObject info;

    info.setProperty (Identifier ("total_samples"),
                      var (int (numTrials)));

    Array<var> bin_edges;
    Array<var> bin_counts;

    for (int bin = 0; bin < binEdges.size() - 1; bin++)
    {
        bin_edges.add (binEdges[bin]);
        bin_counts.add (counts[bin]);
    }

    info.setProperty (Identifier ("bin_edges"), bin_edges);
    info.setProperty (Identifier ("counts"), bin_counts);

    return info;
}