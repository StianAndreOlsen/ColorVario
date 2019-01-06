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
	auto height = size.height / 4;

	auto offsetLabel = TextLabel::New("Offset");
	offsetLabel.SetSize(width, height);
	offsetLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	offsetLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	offsetLabel.SetPosition(0, 0);
	offsetLabel.setVerticalAlignment("CENTER");
	offsetLabel.setHorizontalAlignment("CENTER");
	offsetLabel.setTextColor(Color::mainText());
	offsetLabel.setPointSize(15);
	control.Add(offsetLabel);

	auto altitudeLabel = TextLabel::New("Altitude");
	altitudeLabel.SetSize(width, height);
	altitudeLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	altitudeLabel.SetPosition(0, 0);
	altitudeLabel.setVerticalAlignment("CENTER");
	altitudeLabel.setHorizontalAlignment("CENTER");
	altitudeLabel.setTextColor(Color::error());
//	altitudeLabel.setPointSize(15);
	control.Add(altitudeLabel);

	auto layer = buttonLayer();
	width = size.width / 6;
	height = width;
	auto y = -size.height / 4;
	auto spacing = width;
	auto resourceDir = appSharedResourcePath();

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

	tapDetector = Dali::TapGestureDetector::New();
	tapDetector.Attach(offsetLabel);
	tapDetector.Attach(altitudeLabel);
	tapDetector.DetectedSignal().Connect(this, &AltitudeOffsetDialog::onTapDetected);
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
	setOffset(currentOffset + delta);
}

void Kystsoft::AltitudeOffsetDialog::decrement()
{
	setOffset(currentOffset - delta);
}

void Kystsoft::AltitudeOffsetDialog::onTapDetected(Dali::Actor actor, const Dali::TapGesture& /*gesture*/)
{
	if (actor == offsetLabel())
		setOffset(0);
	else if (actor == altitudeLabel())
		setOffset(-(altitudeWriter.value() - currentOffset));
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
	offsetWriter.setPaper(offsetLabel());
	altitudeWriter.setPaper(altitudeLabel());
	return false;
}
