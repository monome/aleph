/*
  ==============================================================================

    ConnectorComponent.cpp
    Created: 11 Feb 2016 3:11:32am
    Author:  ezra

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ConnectorComponent.h"

//==============================================================================
ConnectorComponent::ConnectorComponent() :
    dragging_(false)
{
}

ConnectorComponent::~ConnectorComponent()
{
}

void ConnectorComponent::paint (Graphics& g)
{
    g.setColour(Colours::black);
    if(slope_) { 
        g.drawLine(0, 0, getWidth(), getHeight(), 2);
    } else {
        g.drawLine(0, getHeight(), getWidth(), 0, 2);
    }
}

void ConnectorComponent::resized()
{
}