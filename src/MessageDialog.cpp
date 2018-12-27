#include "MessageDialog.h"
#include "algorithm.h"

void Kystsoft::MessageDialog::create(const Dali::Vector2& size)
{
	Dialog::create(size);

	auto textViewSize = Dali::Vector2(size.width * 4 / 6, size.height);
	textView.create(textViewSize);
	control.Add(textView);
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
	if (std::find(messages.cbegin(), messages.cend(), message) != messages.cend())
		return true;
	return false;
}

bool Kystsoft::MessageDialog::add(const Message& message)
{
	if (contains(message))
		return false;
	messages.push_back(message);
	return true;
}

bool Kystsoft::MessageDialog::remove(const Message& message)
{
	auto count = messages.size();
	std::remove(messages.begin(), messages.end(), message);
	return messages.size() < count;
}

void Kystsoft::MessageDialog::showMessage(size_t index)
{
	if (messages.empty())
		return;
	index = std::clamp(index, 0u, messages.size() - 1);
	textView.setText(messages[index].text());
}
