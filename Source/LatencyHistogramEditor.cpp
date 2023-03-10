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

#include "LatencyHistogramEditor.h"
#include "LatencyHistogram.h"

LatencyHistogramEditor::LatencyHistogramEditor(GenericProcessor* parentNode) 
    : GenericEditor(parentNode)
{
    
    addComboBoxParameterEditor("ttl_a", 15, 35);
    addComboBoxParameterEditor("ttl_b", 15, 75);
    
    desiredWidth = 270;

}


void LatencyHistogramEditor::updateSettings()
{

	histograms.clear();

	for (auto stream : getProcessor()->getDataStreams())
	{
		Histogram* h = new Histogram(stream->getStreamId());
		histograms.add(h);
		h->setBounds(115, 30, 150, 90);
		addAndMakeVisible(h);
	}

	selectedStreamHasChanged();

}

void LatencyHistogramEditor::selectedStreamHasChanged()
{

	for (auto hist : histograms)
	{
		if (hist->streamId == selectedStream)
		{
			hist->setVisible(true);
		}
		else
		{
			hist->setVisible(false);
		}
	}
}

void LatencyHistogramEditor::addLatency(uint16 streamId, double latencyMs)
{
	for (auto hist : histograms)
	{
		if (hist->streamId == streamId)
		{
			hist->addLatency(latencyMs);
		}
	}
}

void LatencyHistogramEditor::startAcquisition()
{
	for (auto hist : histograms)
	{
		hist->startTimer(100);
	}
}

void LatencyHistogramEditor::stopAcquisition()
{
	for (auto hist : histograms)
	{
		hist->stopTimer();
	}
}