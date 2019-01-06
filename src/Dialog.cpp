#include "Dialog.h"
#include "AppFunctions.h"

void Kystsoft::Dialog::create(const Dali::Vector2& size)
{
	control = Dali::Toolkit::Control::New();
	control.SetSize(size);
	control.SetParentOrigin(Dali::ParentOrigin::CENTER);
	control.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	control.SetPosition(0, 0);
	control.SetBackgroundColor(Color::window());
	control.SetVisible(false);
	control.KeyEventSignal().Connect(this, &Dialog::onKeyEvent);

	auto layer = Dali::Layer::New();
	layer.SetSize(size);
	layer.SetParentOrigin(Dali::ParentOrigin::CENTER);
	layer.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	layer.SetPosition(0, 0);
	control.Add(layer);
	layer.RaiseToTop();

	auto width = size.width / 6;
	auto height = size.height;
	auto resourceDir = appSharedResourcePath();

	acceptButton = PushButton::New();
	acceptButton.SetSize(width, height);
	acceptButton.SetParentOrigin(Dali::ParentOrigin::CENTER_RIGHT);
	acceptButton.SetAnchorPoint(Dali::AnchorPoint::CENTER_RIGHT);
	acceptButton.SetPosition(0, 0);
	acceptButton.setUnselectedImage(resourceDir + "Accept.png");
	acceptButton.setSelectedImage(resourceDir + "AcceptPressed.png");
	acceptButton.SetVisible(false);
	acceptButton.ClickedSignal().Connect(this, &Dialog::onAcceptButtonClicked);
	layer.Add(acceptButton);

	rejectButton = PushButton::New();
	rejectButton.SetSize(width, height);
	rejectButton.SetParentOrigin(Dali::ParentOrigin::CENTER_LEFT);
	rejectButton.SetAnchorPoint(Dali::AnchorPoint::CENTER_LEFT);
	rejectButton.SetPosition(0, 0);
	rejectButton.setUnselectedImage(resourceDir + "Reject.png");
	rejectButton.setSelectedImage(resourceDir + "RejectPressed.png");
	rejectButton.SetVisible(false);
	rejectButton.ClickedSignal().Connect(this, &Dialog::onRejectButtonClicked);
	layer.Add(rejectButton);

	width = size.width;
	height = size.height / 6;

	closeButton = PushButton::New();
	closeButton.SetSize(width, height);
	closeButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	closeButton.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	closeButton.SetPosition(0, 0);
	closeButton.setUnselectedImage(resourceDir + "Close.png");
	closeButton.setSelectedImage(resourceDir + "ClosePressed.png");
	closeButton.SetVisible(false);
	closeButton.ClickedSignal().Connect(this, &Dialog::onCloseButtonClicked);
	layer.Add(closeButton);
}

void Kystsoft::Dialog::setVisible(bool visible)
{
//	auto fromScale = Dali::Vector3(0.5f, 0.5f, 0.5f); // logically best, but since the dialog is rendered as a square, we
	auto fromScale = Dali::Vector3(2.0f, 2.0f, 2.0f); // zoom the other way to avoid showing the stuff outside the circle
	auto toScale = fromScale;
	auto fromAlpha = 0.0f;
	auto toAlpha = fromAlpha;
	if (visible)
	{
		toScale = Dali::Vector3(1, 1, 1);
		toAlpha = 1;
	}
	else
	{
		fromScale = Dali::Vector3(1, 1, 1);
		fromAlpha = 1;
	}
	control.SetScale(fromScale);
	control.SetOpacity(fromAlpha);
	control.SetVisible(true);
	auto layer = control.GetLayer();
	layer.SetVisible(true);
	auto animation = Dali::Animation::New(0.25f);
	animation.AnimateTo(Dali::Property(control, Dali::Actor::Property::SCALE), toScale);
	animation.AnimateTo(Dali::Property(control, Dali::Actor::Property::COLOR_ALPHA), toAlpha);
	animation.AnimateTo(Dali::Property(control, Dali::Actor::Property::VISIBLE), visible);
	animation.AnimateTo(Dali::Property(layer, Dali::Actor::Property::VISIBLE), visible);
	animation.FinishedSignal().Connect(this, &Dialog::onAnimationFinished);
	animation.Play();

	if (visible)
		control.SetKeyInputFocus();
	else
		control.ClearKeyInputFocus();
}

bool Kystsoft::Dialog::onKeyEvent(Dali::Toolkit::Control /*control*/, const Dali::KeyEvent& event)
{
	if (event.state == Dali::KeyEvent::Up)
	{
		if (Dali::IsKey(event, Dali::DALI_KEY_ESCAPE) ||
		    Dali::IsKey(event, Dali::DALI_KEY_BACK))
		{
			close();
			return true;
		}
	}
	return false;
}
