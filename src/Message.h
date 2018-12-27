#ifndef KYSTSOFT_MESSAGE_H
#define KYSTSOFT_MESSAGE_H

#include <string>

namespace Kystsoft {

class Message
{
public:
	enum class Type
	{
		Information,
		Success,
		Warning,
		Error
	};
	Message(Type type, const std::string& text) : typ(type), txt(text) {}
	Message(Type type, const std::string& heading, const std::string& body);
	bool operator==(const Message& other) const { return typ == other.typ && txt == other.txt; }
	static Message information(const std::string& text) { return Message(Type::Information, text); }
	static Message success(const std::string& text) { return Message(Type::Success, text); }
	static Message warning(const std::string& text) { return Message(Type::Warning, text); }
	static Message error(const std::string& text) { return Message(Type::Error, text); }
	static Message information(const std::string& heading, const std::string& body) { return Message(Type::Information, heading, body); }
	static Message success(const std::string& heading, const std::string& body) { return Message(Type::Success, heading, body); }
	static Message warning(const std::string& heading, const std::string& body) { return Message(Type::Warning, heading, body); }
	static Message error(const std::string& heading, const std::string& body) { return Message(Type::Error, heading, body); }
	Type type() const { return typ; }
	void setType(Type type) { typ = type; }
	std::string text() const { return txt; }
	void setText(const std::string& text) { txt = text; }
private:
	Type typ;
	std::string txt;
};

} // namespace Kystsoft

#endif // KYSTSOFT_MESSAGE_H
