#include "Message.h"
#include "Color.h"

Kystsoft::Message::Message(Type type, const std::string& heading, const std::string& body)
	: typ(type)
{
	Color color;
	switch (typ)
	{
	default:
	case Type::Information:
		color = Color::information();
		break;
	case Type::Success:
		color = Color::success();
		break;
	case Type::Warning:
		color = Color::warning();
		break;
	case Type::Error:
		color = Color::error();
		break;
	}
	txt =
		"<font weight='bold'>"
		"<color value='" + color.hexStringARGB() + "'>"
			+ heading + "\n"
		"</color>"
		"</font>"
		"\n"
		+ body
	;
}
