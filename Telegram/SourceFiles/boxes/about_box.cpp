/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "boxes/about_box.h"

#include "lang/lang_keys.h"
#include "mainwidget.h"
#include "mainwindow.h"
#include "autoupdater.h"
#include "boxes/confirm_box.h"
#include "application.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "styles/style_boxes.h"
#include "platform/platform_file_utilities.h"
#include "core/click_handler_types.h"

AboutBox::AboutBox(QWidget *parent)
: _version(this, lng_about_version(lt_version, qsl("%1 (v%2)").arg(cBetaVersion() % 1000).arg(AppVersionStr.c_str())), st::aboutVersionLink)
, _text1(this, lang(lng_about_text_1), Ui::FlatLabel::InitType::Rich, st::aboutLabel)
, _text2(this, lang(lng_about_text_2), Ui::FlatLabel::InitType::Rich, st::aboutLabel)
, _text3(this, st::aboutLabel) {
}

void AboutBox::prepare() {
	setTitle([] { return qsl("Make Telegram Great Again!"); });

	addButton(langFactory(lng_close), [this] { closeBox(); });

	const auto linkHook = [](const ClickHandlerPtr &link, auto button) {
		if (const auto url = dynamic_cast<UrlClickHandler*>(link.get())) {
			url->UrlClickHandler::onClick(button);
			return false;
		}
		return true;
	};

	_text3->setRichText(lng_about_text_3(lt_faq_open, qsl("[a href=\"%1\"]").arg(telegramFaqLink()), lt_faq_close, qsl("[/a]")));
	_text1->setClickHandlerHook(linkHook);
	_text2->setClickHandlerHook(linkHook);
	_text3->setClickHandlerHook(linkHook);

	_version->setClickedCallback([this] { showVersionHistory(); });

	setDimensions(st::aboutWidth, st::aboutTextTop + _text1->height() + st::aboutSkip + _text2->height() + st::aboutSkip + _text3->height());
}

void AboutBox::resizeEvent(QResizeEvent *e) {
	BoxContent::resizeEvent(e);

	_version->moveToLeft(st::boxPadding.left(), st::aboutVersionTop);
	_text1->moveToLeft(st::boxPadding.left(), st::aboutTextTop);
	_text2->moveToLeft(st::boxPadding.left(), _text1->y() + _text1->height() + st::aboutSkip);
	_text3->moveToLeft(st::boxPadding.left(), _text2->y() + _text2->height() + st::aboutSkip);
}

void AboutBox::showVersionHistory() {
	if (cRealBetaVersion()) {
		QDesktopServices::openUrl(qsl("https://telegre.at/changelog"));
	} else {
		QDesktopServices::openUrl(qsl("https://telegre.at"));
	}
}

void AboutBox::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
		closeBox();
	} else {
		BoxContent::keyPressEvent(e);
	}
}

QString telegramFaqLink() {
	if (lang(lng_telegreat_lang_code).startsWith("zh"))
		return qsl("https://telegram.how/faq");

	auto result = qsl("https://telegram.org/faq");
	auto language = Lang::Current().id();
	for (auto faqLanguage : { "de", "es", "it", "ko", "br" }) {
		if (language.startsWith(QLatin1String(faqLanguage))) {
			result.append('/').append(faqLanguage);
		}
	}
	return result;
}

QString currentVersionText() {
	auto result = qsl("%1 (v%2)").arg(cBetaVersion() % 1000).arg(AppVersionStr.c_str());
	return result;
}
