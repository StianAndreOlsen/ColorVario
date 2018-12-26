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
	bool operator==(const Message& other) const { return typ == other.typ && txt == other.txt; }
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
