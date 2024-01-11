/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2013 Open Ephys

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

#ifndef Histogram_H__
#define Histogram_H__

#include <EditorHeaders.h>

#include <vector>

/**
 
    Displays the latency histogram
 
 */
class Histogram : public Component, 
    public Timer
{
public:
    
    /** Constructor */
    Histogram(uint16 streamId);
    
    /** Destructor */
    ~Histogram() { }
    
    /** Draws the histogram */
    void paint(Graphics& g);
    
    /** Called when histogram is resized */
    void resized();
    
    /** Adds a latency */
    void addLatency(double latencyInMs);
    
    /** Clears the display*/
    void clear();

    /** Saves the histogram data*/
    void save();
    
    /** Sets the window size*/
    void setWindowSizeMs(int pre_ms, int post_ms);
    
    /** Sets the bin size in ms*/
    void setBinSizeMs(int ms);

    /** Listens for mouse movements */
    void mouseMove(const MouseEvent& event);
    
    /** Listens for mouse movements */
    void mouseExit(const MouseEvent& event);

    /** Listens for mouse clicks */
    void mouseDown(const MouseEvent& event);
    
    /** Stream ID for this histogram */
    uint16 streamId;

    /** Spike channel for this histogram */
    const SpikeChannel* spikeChannel;

    /** Return info about this histogram */
    DynamicObject getInfo();

private:

    /** Animates the component */
	void timerCallback();
    
    /** Recomputes temporal offsets */
    void recompute();
    
    /** Recomputes bin counts */
    void recount(bool full=true);
    
    std::unique_ptr<Label> hoverLabel;
    std::unique_ptr<Label> statsLabel;
    
    Array<double> binEdges;
    
    Array<double> relativeTimes;
    
    Array<int> counts;

    Colour baseColour;
    int hoverBin = -1;
    
    int pre_ms;
    int post_ms;
    int bin_size_ms;

    float histogramWidth;
    float histogramHeight;

    bool refreshNeeded = true;

    double mean = 0.0f;
    double std = 0.0f;
    float numTrials = 0;

    int maxCount = 1;

};



#endif  // Histogram_H__
