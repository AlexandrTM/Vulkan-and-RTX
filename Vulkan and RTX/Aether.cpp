#include "pch.h"
#include "AetherEngine.h"

static void customQtMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
	QByteArray localMsg = msg.toLocal8Bit();

	// Reformat Vulkan validation messages
	if (msg.contains("Validation")) {
		QByteArray localMsg = msg.toLocal8Bit();
		const char* logMessage = localMsg.constData();

		// Remove "vkDebug: " if it exists
		std::string formattedMsg = logMessage;
		std::string prefix = "vkDebug: ";
		size_t pos = formattedMsg.find(prefix);
		if (pos != std::string::npos) {
			formattedMsg = formattedMsg.substr(pos + prefix.length()); // Remove prefix
		}

		fprintf(stderr, "\033[33m%s\033[0m\n\n", formattedMsg.c_str()); // Yellow color
		return;
	}

	// Forward all other messages to Qt's default handler
	switch (type) {
	case QtDebugMsg:
		QMessageLogger(context.file, context.line, context.function).debug() << msg;
		break;
	case QtWarningMsg:
		QMessageLogger(context.file, context.line, context.function).warning() << msg;
		break;
	case QtCriticalMsg:
		QMessageLogger(context.file, context.line, context.function).critical() << msg;
		break;
	case QtFatalMsg:
		QMessageLogger(context.file, context.line, context.function).fatal("%s", msg.toUtf8().constData());
		abort();
	}
}

static int runAether() {
	qInstallMessageHandler(customQtMessageHandler);

	int argc = 0;
	char** argv = nullptr;

	/*
	Style	 Look			 Performance	Customizable	Good For
	Basic	 Plain		     ⭐⭐⭐⭐⭐	    ✅✅✅✅✅	Embedded, full control
	Material Modern (Google) ⭐⭐⭐	        ✅✅✅			Mobile/modern UI
	Imagine	 Themed (SVG)	 ⭐⭐	        🎨				Unlimited	Games, stylized UIs
	Fusion	 Neutral	     ⭐⭐⭐⭐	        ✅✅✅✅		Cross-platform consistency
	*/

	QQuickStyle::setStyle("Material");
	QApplication app(argc, argv);

	srand(static_cast<unsigned>(time(0))); // Seed the random number generator once
	AetherEngine aetherEngine;

	try {
		aetherEngine.run();
		return EXIT_SUCCESS;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}

int main() {
	return runAether();
}