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

#ifndef LATENCYHISTOGRAMEDITOR_H_DEFINED
#define LATENCYHISTOGRAMEDITOR_H_DEFINED

#include <EditorHeaders.h>

#include "Histogram.h"

class LatencyHistogramEditor : public GenericEditor
{
public:

	/** Constructor */
	LatencyHistogramEditor(GenericProcessor* parentNode);

	/** Destructor */
	~LatencyHistogramEditor() { }

	/** Updates histograms for new streams */
	void updateSettings();

	/** Changes active histogram when selected stream changes */
	void selectedStreamHasChanged();

	/** Starts animation timer */
	void startAcquisition() override;

	/** Stops animation timer */
	void stopAcquisition() override;
	
	/** Adds a new latency value*/
	void addLatency(uint16 streamId, double latencyMs);

private:

	/** Stores the current histograms */
	OwnedArray<Histogram> histograms;

	/** Generates an assertion if this class leaks */
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LatencyHistogramEditor);
};

#endif // LatencyHistogramEDITOR_H_DEFINED