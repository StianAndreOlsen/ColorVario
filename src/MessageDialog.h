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
	void setVisible(bool visible) override;
	bool hasMessagesOfType(Message::Type type) const;
	bool hasInformationMessages() const { return hasMessagesOfType(Message::Type::Information); }
	bool hasSuccessMessages() const { return hasMessagesOfType(Message::Type::Success); }
	bool hasWarningMessages() const { return hasMessagesOfType(Message::Type::Warning); }
	bool hasErrorMessages() const { return hasMessagesOfType(Message::Type::Error); }
	bool contains(const Message& message) const;
	bool add(const Message& message);
	bool remove(const Message& message);
	int messageCount() const { return static_cast<int>(messages.size()); }
	int currentMessageIndex() const { return currentMessage; }
	void showMessage(int messageIndex);
	void showLastMessage() { showMessage(messageCount() - 1); }
	void onWheelEvent(const Dali::WheelEvent& event) { textView.onWheelEvent(event); }
private:
	void updateButtons();
	bool onNextButtonClicked(Dali::Toolkit::Button button);
	bool onPreviousButtonClicked(Dali::Toolkit::Button button);
	std::vector<Message> messages;
	int currentMessage = -1; // current message must be initialized using showMessage
	TextView textView;
	PushButton nextButton;
	PushButton previousButton;
};

} // namespace Kystsoft

#endif // KYSTSOFT_MESSAGEDIALOG_H
