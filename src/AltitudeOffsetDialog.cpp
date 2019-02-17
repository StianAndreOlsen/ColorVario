#include "AltitudeOffsetDialog.h"
#include "AppFunctions.h"

void Kystsoft::AltitudeOffsetDialog::create(const Dali::Vector2& size)
{
	Dialog::create(size);
	acceptButton.SetVisible(true);
	rejectButton.SetVisible(true);

	visibleSignal().connect(this, &AltitudeOffsetDialog::onVisible);
	visibleTimer = Dali::Timer::New(250);
	visibleTimer.TickSignal().Connect(this, &AltitudeOffsetDialog::onVisibleTimer);

	auto width = size.width;
	auto height = size.height / 6;
	auto resourceDir = appSharedResourcePath();

	auto arrows = Dali::Toolkit::ImageView::New(resourceDir + "Arrows.png");
	arrows.SetSize(width, height);
	arrows.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	arrows.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	arrows.SetPosition(0, 0);
	control.Add(arrows);

	width = size.width;
	height = size.height / 4;

	offsetLabl = TextLabel::New("Offset");
	offsetLabl.SetSize(width, height);
	offsetLabl.SetParentOrigin(Dali::ParentOrigin::CENTER);
	offsetLabl.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	offsetLabl.SetPosition(0, 0);
	offsetLabl.setVerticalAlignment("CENTER");
	offsetLabl.setHorizontalAlignment("CENTER");
	offsetLabl.setTextColor(Color::mainText());
	offsetLabl.setPointSize(15);
	control.Add(offsetLabl);

	altitudeLabl = TextLabel::New("Altitude");
	altitudeLabl.SetSize(width, height);
	altitudeLabl.SetParentOrigin(Dali::ParentOrigin::CENTER);
	altitudeLabl.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	altitudeLabl.SetPosition(0, 0);
	altitudeLabl.setVerticalAlignment("CENTER");
	altitudeLabl.setHorizontalAlignment("CENTER");
	altitudeLabl.setTextColor(Color::error());
	altitudeLabl.setPointSize(13);
	control.Add(altitudeLabl);

	width = size.width * 4 / 6;
	height = size.height / 4;

	auto tipLabel = TextLabel::New("Press and hold\nto reset");
	tipLabel.SetSize(width, height);
	tipLabel.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	tipLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	tipLabel.SetPosition(0, 0);
	tipLabel.setVerticalAlignment("CENTER");
	tipLabel.setHorizontalAlignment("CENTER");
	tipLabel.setTextColor(Color::disabled());
	tipLabel.setPointSize(7);
	tipLabel.setMultiLine(true);
	control.Add(tipLabel);

	auto layer = buttonLayer();
	width = size.width / 6;
	height = width;
	auto y = -size.height / 4;
	auto spacing = width / 2;

	incrementButton = PushButton::New();
	incrementButton.SetSize(width, height);
	incrementButton.SetParentOrigin(Dali::ParentOrigin::CENTER);
	incrementButton.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_LEFT);
	incrementButton.SetPosition(spacing / 2, y);
	incrementButton.setUnselectedImage(resourceDir + "Increment.png");
	incrementButton.setSelectedImage(resourceDir + "IncrementPressed.png");
	incrementButton.ClickedSignal().Connect(this, &AltitudeOffsetDialog::onIncrementButtonClicked);
	control.Add(incrementButton);

	decrementButton = PushButton::New();
	decrementButton.SetSize(width, height);
	decrementButton.SetParentOrigin(Dali::ParentOrigin::CENTER);
	decrementButton.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_RIGHT);
	decrementButton.SetPosition(-spacing / 2, y);
	decrementButton.setUnselectedImage(resourceDir + "Decrement.png");
	decrementButton.setSelectedImage(resourceDir + "DecrementPressed.png");
	decrementButton.ClickedSignal().Connect(this, &AltitudeOffsetDialog::onDecrementButtonClicked);
	control.Add(decrementButton);

	longPressDetector = Dali::LongPressGestureDetector::New();
	longPressDetector.Attach(acceptButton);
	longPressDetector.Attach(rejectButton);
	longPressDetector.Attach(offsetLabl);
	longPressDetector.Attach(altitudeLabl);
	longPressDetector.DetectedSignal().Connect(this, &AltitudeOffsetDialog::onLongPressDetected);
}

void Kystsoft::AltitudeOffsetDialog::load(const Settings& settings)
{
	offsetWriter.load(settings);
	altitudeWriter.load(settings);
}

void Kystsoft::AltitudeOffsetDialog::reject()
{
	setOffset(initialOffset);
	Dialog::reject();
}

void Kystsoft::AltitudeOffsetDialog::close()
{
	setOffset(initialOffset);
	Dialog::close();
}

void Kystsoft::AltitudeOffsetDialog::setOffset(double offset)
{
	if (offsetWriter.offset() != offset)
	{
		offsetWriter.setOffset(offset);
		offsetChangedSignl.emit(offset);
	}
}

void Kystsoft::AltitudeOffsetDialog::setAltitude(double altitude)
{
	altitudeWriter.setAltitude(altitude);
}

void Kystsoft::AltitudeOffsetDialog::onWheelEvent(const Dali::WheelEvent& event)
{
	if (event.z > 0)
		increment();
	else if (event.z < 0)
		decrement();
}

void Kystsoft::AltitudeOffsetDialog::increment()
{
	offsetWriter.incrementOffset();
	offsetChangedSignl.emit(offsetWriter.offset());
}

void Kystsoft::AltitudeOffsetDialog::decrement()
{
	offsetWriter.decrementOffset();
	offsetChangedSignl.emit(offsetWriter.offset());
}

void Kystsoft::AltitudeOffsetDialog::onLongPressDetected(Dali::Actor actor, const Dali::LongPressGesture& gesture)
{
	if (gesture.state == Dali::Gesture::Started)
	{
		if (actor == offsetLabl)
			setOffset(0);
		else if (actor == altitudeLabl)
			setOffset(-(altitudeWriter.altitude() - offsetWriter.offset()));
	}
}

void Kystsoft::AltitudeOffsetDialog::onVisible(bool visible)
{
	if (visible)
	{
		visibleTimer.Start();
		initialOffset = offsetWriter.offset();
	}
	else
	{
		visibleTimer.Stop();
		offsetWriter.setPaper();
		altitudeWriter.setPaper();
	}
}

bool Kystsoft::AltitudeOffsetDialog::onVisibleTimer()
{
	offsetWriter.setPaper(offsetLabl);
	altitudeWriter.setPaper(altitudeLabl);
	return false;
}
