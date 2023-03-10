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

#ifndef LATENCYHISTOGRAM_H_DEFINED
#define LATENCYHISTOGRAM_H_DEFINED

#include <ProcessorHeaders.h>

/** Holds settings for one stream's latency histogram */

class HistogramSettings
{

public:

	/** Constructor -- sets default values*/
	HistogramSettings() 
	{ 
		reset();
	}

	/** Destructor */
	~HistogramSettings() {}

	/** Resets values*/
	void reset()
	{
		lastTimeTTLA = -1111.0f;
	}

	/** Last time for TTL A */
	double lastTimeTTLA;

};

class LatencyHistogram : public GenericProcessor
{
public:
	/** The class constructor, used to initialize any members. */
	LatencyHistogram();

	/** The class destructor, used to deallocate memory */
	~LatencyHistogram();

	/** If the processor has a custom editor, this method must be defined to instantiate it. */
	AudioProcessorEditor* createEditor() override;

	/** Called when settings need to be updated */
	void updateSettings() override;

	/** Called when acquisition begins */
	bool startAcquisition() override;

	/** Defines the functionality of the processor.
		The process method is called every time a new data buffer is available.
		Visualizer plugins typically use this method to send data to the canvas for display purposes */
	void process(AudioBuffer<float>& buffer) override;

	/** Handles events received by the processor
		Called automatically for each received event whenever checkForEvents() is called from
		the plugin's process() method */
	void handleTTLEvent(TTLEventPtr event) override;

private:

	StreamSettings<HistogramSettings> settings;

	float maxWindowMs = 1000.0f;

};

#endif