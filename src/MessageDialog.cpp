#include "MessageDialog.h"
#include "algorithm.h"
#include "AppFunctions.h"

void Kystsoft::MessageDialog::create(const Dali::Vector2& size)
{
	Dialog::create(size);

	auto textViewSize = Dali::Vector2(size.width * 4 / 6, size.height);
	textView.create(textViewSize);
	textView.setBottomMargin(size.height / 6);
	control.Add(textView);

	auto layer = Dali::Layer::New();
	layer.SetSize(size);
	layer.SetParentOrigin(Dali::ParentOrigin::CENTER);
	layer.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	layer.SetPosition(0, 0);
	layer.RaiseToTop();
	control.Add(layer);

	// resource directory
	auto resourceDir = appSharedResourcePath();

	auto width = size.width / 6;
	auto height = width;
	previousButton = PushButton::New();
	previousButton.SetSize(width, height);
	previousButton.SetParentOrigin(Dali::ParentOrigin::CENTER_LEFT);
	previousButton.SetAnchorPoint(Dali::AnchorPoint::CENTER_LEFT);
	previousButton.SetPosition(0, 0);
	previousButton.setUnselectedImage(resourceDir + "Previous.png");
	previousButton.setSelectedImage(resourceDir + "PreviousPressed.png");
	previousButton.SetVisible(false);
	previousButton.ClickedSignal().Connect(this, &MessageDialog::onPreviousButtonClicked);
	layer.Add(previousButton);

	nextButton = PushButton::New();
	nextButton.SetSize(width, height);
	nextButton.SetParentOrigin(Dali::ParentOrigin::CENTER_RIGHT);
	nextButton.SetAnchorPoint(Dali::AnchorPoint::CENTER_RIGHT);
	nextButton.SetPosition(0, 0);
	nextButton.setUnselectedImage(resourceDir + "Next.png");
	nextButton.setSelectedImage(resourceDir + "NextPressed.png");
	nextButton.SetVisible(false);
	nextButton.ClickedSignal().Connect(this, &MessageDialog::onNextButtonClicked);
	layer.Add(nextButton);

	width = size.width;
	height = size.height / 6;
	closeButton.SetSize(width, height);
	closeButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	closeButton.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	closeButton.SetPosition(0, 0);
	closeButton.setUnselectedImage(resourceDir + "Close.png");
	closeButton.setSelectedImage(resourceDir + "ClosePressed.png");
	layer.Add(closeButton);

	updateButtons();
}

void Kystsoft::MessageDialog::setVisible(bool visible)
{
	Dialog::setVisible(visible);

	// for some reason, TextView is not scrolling when invisible
	if (visible)
		textView.scrollToTop();
}

bool Kystsoft::MessageDialog::hasMessagesOfType(Message::Type type) const
{
	for (const auto& message : messages)
		if (message.type() == type)
			return true;
	return false;
}

bool Kystsoft::MessageDialog::contains(const Message& message) const
{
	return std::find(messages.cbegin(), messages.cend(), message) != messages.cend();
}

bool Kystsoft::MessageDialog::add(const Message& message)
{
	if (contains(message))
		return false;
	messages.push_back(message);
	showLastMessage();
	return true;
}

bool Kystsoft::MessageDialog::remove(const Message& message)
{
	auto count = messages.size();
	// https://en.wikipedia.org/wiki/Erase%E2%80%93remove_idiom
	messages.erase(std::remove(messages.begin(), messages.end(), message), messages.end());
	if (messages.size() < count)
	{
		showMessage(currentMessage);
		return true;
	}
	return false;
}

void Kystsoft::MessageDialog::showMessage(int messageIndex)
{
	if (messages.empty())
		return;
	currentMessage = std::clamp(messageIndex, 0, messageCount() - 1);
	textView.setText(messages[currentMessage].text());
	updateButtons();
}

void Kystsoft::MessageDialog::updateButtons()
{
	previousButton.SetVisible(currentMessage > 0);
	nextButton.SetVisible(currentMessage < messageCount() - 1);
}

bool Kystsoft::MessageDialog::onPreviousButtonClicked(Dali::Toolkit::Button)
{
	showMessage(currentMessage - 1);
	return true;
}

bool Kystsoft::MessageDialog::onNextButtonClicked(Dali::Toolkit::Button)
{
	showMessage(currentMessage + 1);
	return true;
}
