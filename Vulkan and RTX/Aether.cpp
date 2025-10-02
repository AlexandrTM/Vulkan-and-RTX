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

static void loadTranslation(QApplication& app) {
	QString locale = QLocale::system().name();
	QString languageCode = locale.left(2);

	QTranslator* translator = new QTranslator(&app);

	// try full locale
	if (translator->load("translations/myapp_" + locale)) {
		std::cout << "Loaded translation: myapp_" + locale.toStdString() << "\n";
		app.installTranslator(translator);
		return;
	}

	// try language code
	if (translator->load("translations/myapp_" + languageCode)) {
		std::cout << "Loaded translation: myapp_" + languageCode.toStdString() << "\n";
		app.installTranslator(translator);
		return;
	}

	// default english
	if (translator->load("translations/myapp_en.qm")) {
		std::cout << "Loaded translation: myapp_en.qm\n";
		app.installTranslator(translator);
	}
	else {
		qWarning() << "Failed to load translation file.";
	}
}

static int runAether() {
	/*
	Style	 Look			 Performance	Customizable	Good For
	Basic	 Plain		     ⭐⭐⭐⭐⭐	    ✅✅✅✅✅	Embedded, full control
	Material Modern (Google) ⭐⭐⭐	        ✅✅✅			Mobile/modern UI
	Imagine	 Themed (SVG)	 ⭐⭐	        🎨				Unlimited	Games, stylized UIs
	Fusion	 Neutral	     ⭐⭐⭐⭐	        ✅✅✅✅		Cross-platform consistency
	*/

	qInstallMessageHandler(customQtMessageHandler);

	int argc = 0;
	char** argv = nullptr;

	QQuickStyle::setStyle("Material");
	QApplication app(argc, argv);

	loadTranslation(app);

	seedRandomGenerator();

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
