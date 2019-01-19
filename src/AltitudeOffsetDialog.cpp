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

	offsetWriter.setTitle("Offset");
	offsetWriter.setAlwaysShowSign(true);
	offsetWriter.setRoundToNearest(1);
	altitudeWriter.setRoundToNearest(1);

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
//	altitudeLabl.setPointSize(15);
	control.Add(altitudeLabl);

	width = size.width * 4 / 6;
	height = size.height / 4;

	// TODO: Choose the best term!
	auto tipLabel = TextLabel::New("Press and hold\nto reset");
//	auto tipLabel = TextLabel::New("Long press\nto reset");
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

void Kystsoft::AltitudeOffsetDialog::reject()
{
	setOffset(initialOffset);
	Dialog::reject();
}

void Kystsoft::AltitudeOffsetDialog::close()
{
	// TODO: Discuss with Kyrre if close should keep the value
	setOffset(initialOffset);
	Dialog::close();
}

void Kystsoft::AltitudeOffsetDialog::setOffset(double offset)
{
	if (currentOffset != offset)
	{
		currentOffset = offset;
		offsetWriter.setValue(offset);
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
	updateDelta();
	setOffset(currentOffset + currentDelta);
}

void Kystsoft::AltitudeOffsetDialog::decrement()
{
	updateDelta();
	setOffset(currentOffset - currentDelta);
}

void Kystsoft::AltitudeOffsetDialog::updateDelta()
{
	auto timeElapsed = deltaTimer.seconds();
	if (timeElapsed > 2.5)
		currentDelta = initialDelta;
	else if (timeElapsed > 1.5 && currentDelta > 5 * initialDelta)
		currentDelta /= 10;
	else if (timeElapsed < 0.5 && currentDelta < 500 * initialDelta)
	{
		auto x = currentOffset;
		auto m = currentDelta;
		if (std::fabs(mround(x, 10 * m) - mround(x, m)) < m / 2)
			currentDelta *= 10;
	}
	deltaTimer.start();
}

void Kystsoft::AltitudeOffsetDialog::onLongPressDetected(Dali::Actor actor, const Dali::LongPressGesture& gesture)
{
	if (gesture.state == Dali::Gesture::Started)
	{
		if (actor == offsetLabl)
			setOffset(0);
		else if (actor == altitudeLabl)
			setOffset(-(altitudeWriter.value() - currentOffset));
	}
}

void Kystsoft::AltitudeOffsetDialog::onVisible(bool visible)
{
	if (visible)
	{
		visibleTimer.Start();
		initialOffset = currentOffset;
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
