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

#include "LatencyHistogram.h"

#include "LatencyHistogramEditor.h"


LatencyHistogram::LatencyHistogram()
    : GenericProcessor("Latency Histogram")
{

    addIntParameter(Parameter::STREAM_SCOPE, "ttl_a", "The first TTL line to measure", 1, 1, 16);
    addIntParameter(Parameter::STREAM_SCOPE, "ttl_b", "The second TTL line to measure", 2, 1, 16);

}


LatencyHistogram::~LatencyHistogram()
{

}


AudioProcessorEditor* LatencyHistogram::createEditor()
{
    editor = std::make_unique<LatencyHistogramEditor>(this);
    return editor.get();
}


void LatencyHistogram::updateSettings()
{
    settings.update(getDataStreams()); // resets settings to default values
}


bool LatencyHistogram::startAcquisition()
{
	for (auto stream : getDataStreams())
	{
        settings[stream->getStreamId()]->reset();
	}

    return true;
}


void LatencyHistogram::process(AudioBuffer<float>& buffer)
{

    checkForEvents(true);

}


void LatencyHistogram::handleTTLEvent(TTLEventPtr event)
{
    const bool state = event->getState();

    if (!state)
		return; // only handle rising edges
    
    const uint16 streamId = event->getStreamId();
    
    DataStream* stream = getDataStream(streamId);
    
    if (stream != nullptr)
    {

        // convert to 1-based line numbers
        const int line = event->getLine() + 1;
        double timeInMs = event->getSampleNumber() / stream->getSampleRate() * 1000.0f;
        
		const int ttl_a = stream->getParameter("ttl_a")->getValue();
		const int ttl_b = stream->getParameter("ttl_b")->getValue();

		if (line == ttl_a)
		{
			settings[streamId]->lastTimeTTLA = timeInMs;
		}
		else if (line == ttl_b)
		{

            double latencyMs = timeInMs - settings[streamId]->lastTimeTTLA;
            
            if (latencyMs < maxWindowMs)
            {
				LatencyHistogramEditor* editor = static_cast<LatencyHistogramEditor*>(getEditor());
                editor->addLatency(streamId, latencyMs);
            }

            settings[streamId]->reset();
		}
    }

    
}


