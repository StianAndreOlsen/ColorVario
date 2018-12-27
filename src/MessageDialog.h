#ifndef KYSTSOFT_MESSAGEDIALOG_H
#define KYSTSOFT_MESSAGEDIALOG_H

#include "Dialog.h"
#include "Message.h"
#include "TextView.h"
#include <vector>

namespace Kystsoft {

class MessageDialog : public Dialog
{
public:
	MessageDialog() {}
	void create(const Dali::Vector2& size);
	bool hasMessagesOfType(Message::Type type) const;
	bool hasInformationMessages() const { return hasMessagesOfType(Message::Type::Information); }
	bool hasSuccessMessages() const { return hasMessagesOfType(Message::Type::Success); }
	bool hasWarningMessages() const { return hasMessagesOfType(Message::Type::Warning); }
	bool hasErrorMessages() const { return hasMessagesOfType(Message::Type::Error); }
	bool contains(const Message& message) const;
	bool add(const Message& message);
	bool remove(const Message& message);
	auto messageCount() const { return messages.size(); }
	void showMessage(size_t index);
	void showLastMessage() { showMessage(messageCount() - 1); }
	void onWheelEvent(const Dali::WheelEvent& event) { textView.onWheelEvent(event); }
private:
	std::vector<Message> messages;
	TextView textView;
};

} // namespace Kystsoft

#endif // KYSTSOFT_MESSAGEDIALOG_H
