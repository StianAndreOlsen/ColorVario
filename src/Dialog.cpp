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
}

void Kystsoft::Dialog::setVisible(bool visible)
{
	// TODO: Decide if we want this animation. It's not perfect since the dialog is rendered as a square and not as a circle.
	auto from = Dali::Vector3();
	auto to = Dali::Vector3();
	if (visible)
		to = Dali::Vector3(1, 1, 1);
	else
		from = Dali::Vector3(1, 1, 1);
	control.SetScale(from);
	control.SetVisible(true);
	auto animation = Dali::Animation::New(0.25f);
	animation.AnimateTo(Dali::Property(control, Dali::Actor::Property::SCALE), to);
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
