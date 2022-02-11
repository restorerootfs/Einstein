// ==============================
// File:			TFLSettings
// Project:			Einstein
//
// Copyright 2003-2022 by Paul Guyot (pguyot@kallisys.net).
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

// generated by Fast Light User Interface Designer (fluid) version 1.0400

#include "TFLSettings.h"

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Preferences.H>
#include <FL/filename.H>

#if TARGET_OS_WIN32
#else
#include <unistd.h>
#endif
#include <algorithm>
#include <errno.h>
#include <iterator>
#include <stdio.h>
#include <vector>

#include "Emulator/PCMCIA/TLinearCard.h"
#include "Emulator/PCMCIA/TNE2000Card.h"
#include "Emulator/ROM/TROMImage.h"
#include "app/FLTK/TFLApp.h"

// MARK: - PC Card Settings

TFLPCCardSettings::TFLPCCardSettings(Fl_Preferences& prefs)
{
	mUUID = strdup(prefs.name());
	prefs.get("name", mName, "<unnamed>");
	int type = 0;
	prefs.get("type", type, 0);
	switch (type)
	{
		default:
			SetType(CardType::kUndefined);
			SetTag("????");
			break;
		case 1:
			SetType(CardType::kNetwork);
			SetTag(">NET");
			break;
		case 2:
			SetType(CardType::kLinear);
			SetTag("FLSH");
			if (mImagePath)
				::free(mImagePath);
			prefs.get("imagePath", mImagePath, "");
			break;
	}
	char newTag[32];
	prefs.get("tag", newTag, GetTag(), 30);
	SetTag(newTag);
	prefs.get("keepInSlot", mKeepInSlot, -1);
}

TFLPCCardSettings::TFLPCCardSettings(const char* uuid, const char* name)
{
	mUUID = strdup(uuid);
	mName = strdup(name);
}

TFLPCCardSettings::TFLPCCardSettings()
{
	mUUID = strdup(Fl_Preferences::newUUID());
	mName = strdup("<unnamed>");
}

TFLPCCardSettings*
TFLPCCardSettings::LinkLinearPCCard(const char* inName, const char* inImageFilename)
{
	TFLPCCardSettings* card = new TFLPCCardSettings();
	card->SetName(inName);
	card->SetType(CardType::kLinear);
	card->SetTag("FLSH");
	card->SetImagePath(inImageFilename);
	return card;
}

TFLPCCardSettings*
TFLPCCardSettings::NewLinearPCCard(const char* inName, const char* inImageFilename, KUInt32 inSizeMB)
{
	TLinearCard::CreateImageFile(inName, inImageFilename, inSizeMB);
	TFLPCCardSettings* card = LinkLinearPCCard(inName, inImageFilename);
	return card;
}

TPCMCIACard*
TFLPCCardSettings::GetCard()
{
	if (!mCard)
	{
		switch (mType)
		{
			case CardType::kUndefined:
				if (mImagePath)
					mType = CardType::kLinear; // Fix an old bug that would create new linear cards as kUndefined
				break;
			case CardType::kNetwork:
				mCard = new TNE2000Card();
				break;
			case CardType::kLinear:
				mCard = new TLinearCard(mImagePath);
				break;
		}
	}
	return mCard;
}

TFLPCCardSettings::~TFLPCCardSettings()
{
	if (mUUID)
		::free(mUUID);
	if (mName)
		::free(mName);
	if (mTag)
		::free(mTag);
	if (mCard)
		delete mCard;
}

void
TFLPCCardSettings::WritePrefs(Fl_Preferences& prefs)
{
	prefs.set("name", mName);
	prefs.set("tag", mTag);
	prefs.set("keepInSlot", mKeepInSlot);
	switch (mType)
	{
		case CardType::kUndefined:
			prefs.set("type", 0);
			break;
		case CardType::kNetwork:
			prefs.set("type", 1);
			break;
		case CardType::kLinear:
			prefs.set("type", 2);
			prefs.set("imagePath", mImagePath);
			break;
	}
}

void
TFLPCCardSettings::SetName(const char* inName)
{
	if (mName)
		::free(mName);
	mName = nullptr;
	if (inName)
		mName = strdup(inName);
}

void
TFLPCCardSettings::SetTag(const char* inTag)
{
	if (mTag)
		::free(mTag);
	if (inTag)
		mTag = strdup(inTag);
	else
		mTag = strdup("---");
}

void
TFLPCCardSettings::SetImagePath(const char* inImagePath)
{
	if (mImagePath)
		::free(mImagePath);
	mImagePath = nullptr;
	if (inImagePath)
		mImagePath = strdup(inImagePath);
}

// MARK: - Application Settings

TFLSettings::TFLSettings() = default;

TFLSettings::~TFLSettings() = default;

void
TFLSettings::ShowSettingsPanelModal()
{
	mSettingsPanel->show();
	while (mSettingsPanel->visible())
		Fl::wait();
}

void
TFLSettings::ShowSettingsPanel()
{
	runningMode();
	mSettingsPanel->show();
}

void
TFLSettings::ShowAboutDialog()
{
	if (!mAboutDialog)
		mAboutDialog = createAboutDialog();
	mAboutDialog->show();
}

void
TFLSettings::setAppPath(const char* AppPath)
{
	appPath = strdup(AppPath);
	char* end = (char*) fl_filename_name(appPath);
	if (end)
		*end = 0;
}

void
TFLSettings::loadPreferences()
{
	char buf[FL_PATH_MAX];

	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");

	// general preferences
	prefs.get("dontShow", dontShow, 0);

	// ROM Preferences
	Fl_Preferences rom(prefs, "ROM");
	{
		buf[0] = 0;
		strncat(buf, appPath, sizeof(buf) - 1);
		strncat(buf, "717006", sizeof(buf) - 1);
		rom.get("path", ROMPath, buf);
		rom.get("builtInRex", mUseBuiltinRex, true);
	}

	// Flash Preferences
	Fl_Preferences flash(prefs, "Flash");
	{
		buf[0] = 0;
		prefs.getUserdataPath(buf, FL_PATH_MAX - 15);
		strncat(buf, "internal.flash", sizeof(buf) - 1);
		flash.get("path", FlashPath, buf);
	}

	// screen preferences
	Fl_Preferences screen(prefs, "Screen");
	{
		screen.get("width", screenWidth, 320);
		screen.get("height", screenHeight, 480);
		screen.get("fullScreen", fullScreen, 0);
		screen.get("hideMouse", hideMouse, 0);

		screen.get("AppWindowPosX", mAppWindowPosX, 150);
		screen.get("AppWindowPosY", mAppWindowPosY, 150);
		screen.get("AllowScreenResize", mAllowScreenResize, true);
		screen.get("AllowFullscreen", mAllowFullscreen, true);

		screen.get("LaunchMonitorAtBoot", mLaunchMonitorAtBoot, 0);
		screen.get("BreatAtROMBoot", mBreatAtROMBoot, 0);

#if TARGET_OS_MAC
		screen.get("ShowMenubar", mShowMenubar, 0);
#else
		screen.get("ShowMenubar", mShowMenubar, 1);
#endif
		screen.get("ShowToolbar", mShowToolbar, 1);
	}

	// Memory preferences
	Fl_Preferences memory(prefs, "Memory");
	{
		memory.get("RAMSize", RAMSize, 64);
	}

	// system preferences
	Fl_Preferences newtSystem(prefs, "System");
	{
		newtSystem.get("FetchDateAndTime", mFetchDateAndTime, 1);
	}

	// --- PCMCIA Card settings
	Fl_Preferences pcmcia(prefs, "PCMCIA");

	// --- a list of known PCMCIA Cards
	Fl_Preferences cardlist(pcmcia, "CardList");

	Boolean networkCardFound = false;
	for (int i = 0; i < cardlist.groups(); i++)
	{
		Fl_Preferences cardprefs(cardlist, i);
		TFLPCCardSettings* card = new TFLPCCardSettings(cardprefs);
		mCardList.push_back(card);
		if (strcmp(cardprefs.name(), TFLPCCardSettings::kNetworkUUID) == 0)
		{
			networkCardFound = true;
			// fix a wrong entry, created by older version of Einstein.
			card->SetType(TFLPCCardSettings::CardType::kNetwork);
		}
	}
	if (!networkCardFound)
	{
		TFLPCCardSettings* card = new TFLPCCardSettings(TFLPCCardSettings::kNetworkUUID, "Einstein Network Card");
		card->SetType(TFLPCCardSettings::CardType::kNetwork);
		card->SetTag(">NET");
		mCardList.push_back(card);
	}

	// Newton Internet resources
	Fl_Preferences resources(prefs, "Resources");
	{
		resources.get("UnnaPath", mUnnaPath, "http://www.unna.org/");
		resources.get("MessagepadOrgPath", mMessagepadOrgPath, "http://www.messagepad.org/");
		resources.get("GitRepoPath", mGitRepoPath, "https://github.com/pguyot/Einstein.git");
	}
}

void
TFLSettings::savePreferences()
{
	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");

	// general preferences
	prefs.set("dontShow", dontShow);

	// ROM Preferences
	Fl_Preferences rom(prefs, "ROM");
	{
		rom.set("path", ROMPath);
		rom.set("builtInRex", mUseBuiltinRex);
	}

	// Flash Preferences
	Fl_Preferences flash(prefs, "Flash");
	{
		flash.set("path", FlashPath);
	}

	// screen preferences
	Fl_Preferences screen(prefs, "Screen");
	{
		screen.set("width", screenWidth);
		screen.set("height", screenHeight);
		screen.set("fullScreen", fullScreen);
		screen.set("hideMouse", hideMouse);

		screen.set("AppWindowPosX", mAppWindowPosX);
		screen.set("AppWindowPosY", mAppWindowPosY);
		screen.set("AllowScreenResize", mAllowScreenResize);
		screen.set("AllowFullscreen", mAllowFullscreen);

		screen.set("LaunchMonitorAtBoot", mLaunchMonitorAtBoot);
		screen.set("BreatAtROMBoot", mBreatAtROMBoot);

		screen.set("ShowMenubar", mShowMenubar);
		screen.set("ShowToolbar", mShowToolbar);
	}

	// Memory preferences
	Fl_Preferences memory(prefs, "Memory");
	{
		memory.set("RAMSize", RAMSize);
	}

	// system preferences
	Fl_Preferences newtSystem(prefs, "System");
	{
		newtSystem.set("FetchDateAndTime", mFetchDateAndTime);
	}

	// --- PCMCIA Card settings
	Fl_Preferences pcmcia(prefs, "PCMCIA");

	// --- a list of known PCMCIA Cards
	Fl_Preferences cardlist(pcmcia, "CardList");
	cardlist.clear();

	for (size_t i = 0; i < mCardList.size(); i++)
	{
		TFLPCCardSettings* card = mCardList[i];
		Fl_Preferences cardPrefs(cardlist, card->GetUUID());
		card->WritePrefs(cardPrefs);
	}

	// Newton Internet resources
	Fl_Preferences resources(prefs, "Resources");
	{
		resources.set("UnnaPath", mUnnaPath);
		resources.set("MessagepadOrgPath", mMessagepadOrgPath);
		resources.set("GitRepoPath", mGitRepoPath);
	}
}

const char*
TFLSettings::GetROMDetails(const char* inFilename)
{
	const int text_size = 2 * FL_PATH_MAX;
	static char* text = nullptr;
	if (!text)
	{
		text = (char*) malloc(text_size);
	}

	TROMImage* theROMImage = TROMImage::LoadROMAndREX(inFilename, true, true);
	if (!theROMImage)
	{
		strncpy(text, "Can't load ROM image.\nFile format not supported.", text_size);
		return text;
	}
	if (theROMImage->GetErrorCode() == TROMImage::kNoError)
	{
		switch (theROMImage->GetROMId())
		{
			case TROMImage::k717006:
				strncpy(text,
					"MessagePad MP2x00 US ROM\n"
					"ROM Version 2.1 (717006)-0",
					text_size);
				break;
			case TROMImage::kMP2x00DROM:
				strncpy(text,
					"MessagePad MP2x00 D ROM\n"
					"ROM Version D-2.1 (747129)-0",
					text_size);
				break;
			case TROMImage::kEMate300ROM:
				strncpy(text,
					"eMate 300 ROM\n"
					"ROM Version 2.2.00 (737041)",
					text_size);
				break;
			default:
				strncpy(text,
					"Unknown ROM or not a ROM\n"
					"File content not recognized",
					text_size);
				break;
		}
	} else
	{
		switch (theROMImage->GetErrorCode())
		{
			case TROMImage::kErrorLoadingROMFile:
				snprintf(text, text_size,
					"Can't load ROM file\n%s",
					strerror(errno));
				break;
			case TROMImage::kErrorLoadingNewtonREXFile:
				snprintf(text, text_size,
					"Can't load Newton REX file\n%s",
					strerror(errno));
				break;
			case TROMImage::kErrorLoadingEinsteinREXFile:
				snprintf(text, text_size,
					"Can't load Einstein REX file\n%s",
					strerror(errno));
				break;
			case TROMImage::kErrorWrongSize:
				snprintf(text, text_size,
					"Can't load ROM file\nUnexpected file size.");
				break;
		}
	}
	delete theROMImage;
	return text;
}

TFLPCCardSettings*
TFLSettings::addLinearPCCard(const char* inName, const char* inImageFilename)
{
	return TFLPCCardSettings::LinkLinearPCCard(inName, inImageFilename);
}

static std::vector<TFLPCCardSettings*> gCardList;

/**
 * When creating a new list of PCCards, we must make sure that those cards,
 * that were in the old list, but are no longer used in the new list, will
 * be deleted, so we do not get a memory leak.
 */
void
TFLSettings::startUpdateCardList()
{
	gCardList = mCardList;
	mCardList.clear();
}

/**
 * Make sure that we remove all cards from the old list if they go into the new list.
 */
void
TFLSettings::updateNextCard(const char* inName, TFLPCCardSettings* inCard)
{
	(void) inName;
	mCardList.push_back(inCard);
	auto it = std::find(gCardList.begin(), gCardList.end(), inCard);
	if (it != gCardList.end())
		gCardList.erase(it);
}

/**
 * Delete all cards that are still in the old list.
 */
void
TFLSettings::endUpdateCardList()
{
	// FIXME: make sure that this card is not inserted. If it is, we should mark it as unlisted, so it delets itself when ejected?
	for (auto& card : gCardList)
		delete card;
	gCardList.clear();
}

void
TFLSettings::UnplugPCCard(int ix)
{
	(void) ix;
	// FIXME: write this
}

/*
 * inSlot can be 0 or 1 for the corresponding slot, or -1 if the card must no longer be in any slot
 */
void
TFLSettings::KeepPCCardInSlot(int inSlot, size_t inCard)
{
	Boolean clearIf0 = (inSlot == 0);
	Boolean clearIf1 = (inSlot == 1);

	for (size_t ix = 0; ix < mCardList.size(); ++ix)
	{
		TFLPCCardSettings* card = mCardList[ix];
		if (ix == inCard)
		{
			card->KeepInSlot(inSlot);
		} else
		{
			if (clearIf0 && card->KeepInSlot() == 0)
				card->KeepInSlot(-1);
			if (clearIf1 && card->KeepInSlot() == 1)
				card->KeepInSlot(-1);
		}
	}

	savePreferences();
}

int
TFLSettings::CardToIndex(TPCMCIACard* inCard)
{
	if (!inCard)
		return -1;
	for (size_t ix = 0; ix < mCardList.size(); ++ix)
	{
		TPCMCIACard* card = mCardList[ix]->Card();
		if (card == inCard)
			return ix;
	}
	return -1;
}

int
TFLSettings::GetCardKeptInSlot(int inSlot)
{
	if (inSlot == -1)
		return -1;
	for (size_t ix = 0; ix < mCardList.size(); ++ix)
	{
		if (mCardList[ix]->KeepInSlot() == inSlot)
			return ix;
	}
	return -1;
}
