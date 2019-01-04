#include "Dialog.h"
#include "Color.h"

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

	acceptButton = PushButton::New();
	acceptButton.ClickedSignal().Connect(this, &Dialog::onAcceptButtonClicked);

	rejectButton = PushButton::New();
	rejectButton.ClickedSignal().Connect(this, &Dialog::onRejectButtonClicked);

	closeButton = PushButton::New();
	closeButton.ClickedSignal().Connect(this, &Dialog::onCloseButtonClicked);
}

void Kystsoft::Dialog::setVisible(bool visible)
{
	// TODO: Decide if we want this animation. It's not perfect since the dialog is rendered as a square and not as a circle, and the text is not clipped.
//	auto fromScale = Dali::Vector3(0.5f, 0.5f, 0.5f);
	auto fromScale = Dali::Vector3(2.0f, 2.0f, 2.0f); // this is an alternative to avoid the problems with stuff outside the circle
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
	auto animation = Dali::Animation::New(0.25f);
	animation.AnimateTo(Dali::Property(control, Dali::Actor::Property::SCALE), toScale);
	animation.AnimateTo(Dali::Property(control, Dali::Actor::Property::COLOR_ALPHA), toAlpha);
	animation.AnimateTo(Dali::Property(control, Dali::Actor::Property::VISIBLE), visible);
	animation.Play();

//	control.SetVisible(visible);
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

bool Kystsoft::Dialog::onAcceptButtonClicked(Dali::Toolkit::Button)
{
	accept();
	return true;
}

bool Kystsoft::Dialog::onRejectButtonClicked(Dali::Toolkit::Button)
{
	reject();
	return true;
}

bool Kystsoft::Dialog::onCloseButtonClicked(Dali::Toolkit::Button)
{
	close();
	return true;
}
