#include "pch.h"
#include "CustomUI.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"

using namespace std;

#ifndef IM_PI
#define IM_PI 3.14159265358979323846f
#endif


int CustomUI::intChangePositionX(SettingsItems settings, string settingsName) {
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	if (settingsName == settingsItems) {
		if (showPositionEditor) {
			return changePositionX;
		}
	}
	return settings.int1;
	
}
int CustomUI::intChangePositionY(SettingsItems settings, string settingsName) {
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	if (settingsName == settingsItems) {
		if (showPositionEditor) {
			return changePositionY;
		}
	}
	return settings.int2;

}

float CustomUI::floatChangeSizeX(SettingsItems settings, string settingsName) {
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	if (settingsName == settingsItems) {
		if (showSizeEditor) {
			return changeSizeX;
		}
	}
	return settings.float1;

}
float CustomUI::floatChangeSizeY(SettingsItems settings, string settingsName) {
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	if (settingsName == settingsItems) {
		if (showSizeEditor) {
			return changeSizeY;
		}
	}
	return settings.float2;

}

void CustomUI::Render() {
	

	int idx = ImGui::GetKeyIndex(ImGuiKey_Escape);
	if (ImGui::IsKeyDown(idx))
		escape_state = true;
	else if (ImGui::IsKeyReleased(idx))
		escape_state = false;
	if (isInFreeplay() || isInGame()) {
		RenderWindow();
	}
	if (isSettingsOpen) {
		RenderMenu();
	}
	else {
		showPositionEditor = false;
		showSizeEditor = false;
	}
	//LOG(" " + to_string(isSettingsOpen));
}



// Do ImGui rendering here
void CustomUI::RenderWindow()
{
	// The imgui window allows the quads to show on the screen
	ImVec2 windowPos = ImVec2((0), 0);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(screenSize.X, screenSize.Y));
	//ImGui::SetNextWindowSize(ImVec2(screenSize.X - (1660 * xPercent) + (10 * xPercent), screenSize.Y + (10 * yPercent)));

	static bool no_titlebar = true;
	static bool no_scrollbar = true;
	static bool no_menu = true;
	static bool no_move = true;
	static bool no_resize = true;
	static bool no_collapse = true;
	static bool no_nav = false;
	static bool no_background = true;
	static bool no_bring_to_front = true;

	bool opened = true;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	//if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_)) {
	if (!ImGui::Begin(menuTitle_.c_str(), &opened, window_flags)) {

		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	/*if (!myFont) {
		auto gui = gameWrapper->GetGUIManager();
		myFont = gui.GetFont("Oswald");
	}*/
	//LOG("font size " + to_string(gameWrapper->GetGUIManager().GetFont("Oswald200")->FontSize));

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	if (myFont) {
		ImGui::PushFont(myFont);
		//ImGui::Text("Custom UI Overlay");
	}


	if (isInGame() && gameDisplay) {
		if (!isOnPause && !zeroBoost(boost)) {
			drawBoost(drawList);
		}
		drawScore(drawList);
	}
	else if (isInGame() && replayDisplay && !gameDisplay) {
		drawScore(drawList);
	}
	else if (isInFreeplay() && !zeroBoost(boost) && !isOnPause) {
		drawBoost(drawList);
		//drawScore(drawList);

	}
		
	if (myFont) {
		ImGui::PopFont();
		//ImGui::Text("The custom font haven't been loaded yet");
	}

	ImGui::End();

	//if (!isWindowOpen_)
	//{
	//	_globalCvarManager->executeCommand("togglemenu " + GetMenuName());
	//}

}


void CustomUI::drawScore(ImDrawList* drawList) {


	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	auto& preset = allPresets[keyPreset];
	auto& settingsScoreAllItems = preset.settingsScoreAllItems;
	auto& settingsScoreDisplay = preset.settingsScoreDisplay;
	auto& settingsScoreMyTeam = preset.settingsScoreMyTeam;
	auto& settingsScoreOppositeTeam = preset.settingsScoreOppositeTeam;
	auto& settingsGameTime = preset.settingsGameTime;

	if (!imageScore[keyPreset]->IsLoadedForImGui()) {
		return;
	}

	if (auto renderImageScore = imageScore[keyPreset]->GetImGuiTex()) {
		auto size = imageScore[keyPreset]->GetSizeF();

		const auto& sourceSettings = (settingsItems == "settingsScoreAllItems")
			? settingsScoreAllItems
			: settingsScoreDisplay;
		const auto& baseSettings = (settingsItems == "settingsScoreAllItems")
			? settingsScoreDisplay
			: settingsScoreAllItems;

		const string& stringSettings = (settingsItems == "settingsScoreAllItems")
			? "settingsScoreAllItems"
			: "settingsScoreDisplay";

		ImVec2 position = ImVec2(
			0 + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings),
			0 + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings)
		);

		ImVec2 fullSize = ImVec2(
			size.X * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings),
			size.Y * baseSettings.float2 * floatChangeSizeY(sourceSettings, stringSettings)
		);

		if (preset.format == "custom") {
			drawList->AddImage(
				renderImageScore,
				ImVec2(position.x, position.y),
				ImVec2(position.x + fullSize.x, position.y + fullSize.y),
				ImVec2(0, 0),
				ImVec2(1, 1),
				IM_COL32(255, 255, 255, 255)
			);
		}
		else {
			drawList->AddImage(
				renderImageScore,
				ImVec2(
					0 + intChangePositionX(settingsScoreAllItems, "settingsScoreAllItems"),
					0 + intChangePositionY(settingsScoreAllItems, "settingsScoreAllItems")
				),
				ImVec2(
					(size.X * floatChangeSizeX(settingsScoreAllItems, "settingsScoreAllItems") +
						intChangePositionX(settingsScoreAllItems, "settingsScoreAllItems")),
					(size.Y * floatChangeSizeY(settingsScoreAllItems, "settingsScoreAllItems") +
						intChangePositionY(settingsScoreAllItems, "settingsScoreAllItems"))
				),
				ImVec2(0, 0),
				ImVec2(1, 1),
				IM_COL32(255, 255, 255, 255)
			);
		}
	}

	Vector2 PositionScoreA = { 188, -8 };
	Vector2 PositionGametime = { 283, -10 };
	Vector2 PositionScoreB = { 468, -8 };

	if (isOvertime)
	{
		PositionGametime = { PositionGametime.X - 13 , PositionGametime.Y };
	}
	if (scoreA >= 10)
	{
		PositionScoreA = { PositionScoreA.X - 8 , PositionScoreA.Y };
	}
	if (scoreB >= 10)
	{
		PositionScoreB = { PositionScoreB.X - 8 , PositionScoreB.Y };
	}

	ImU32 colorScoreMyTeam = IM_COL32(allPresets[keyPreset].colorScoreMyTeam[0], allPresets[keyPreset].colorScoreMyTeam[1], allPresets[keyPreset].colorScoreMyTeam[2], allPresets[keyPreset].colorScoreMyTeam[3]);
	ImU32 colorScoreOppositeTeam = IM_COL32(allPresets[keyPreset].colorScoreOppositeTeam[0], allPresets[keyPreset].colorScoreOppositeTeam[1], allPresets[keyPreset].colorScoreOppositeTeam[2], allPresets[keyPreset].colorScoreOppositeTeam[3]);
	ImU32 colorGameTime = IM_COL32(allPresets[keyPreset].colorGameTime[0], allPresets[keyPreset].colorGameTime[1], allPresets[keyPreset].colorGameTime[2], allPresets[keyPreset].colorGameTime[3]);

	drawTextScore(drawList, PositionScoreA, 110, colorScoreMyTeam, settingsScoreAllItems, settingsScoreMyTeam, "settingsScoreMyTeam", to_string(scoreA).c_str());
	drawTextScore(drawList, PositionScoreB, 110, colorScoreOppositeTeam, settingsScoreAllItems, settingsScoreOppositeTeam, "settingsScoreOppositeTeam", to_string(scoreB).c_str());
	drawTextScore(drawList, PositionGametime, 110, colorGameTime, settingsScoreAllItems, settingsGameTime, "settingsScoreOppositeTeam", gameTime);

	/*drawList->AddText(myFont, 110 * xPercent, ImVec2(PositionScoreA.X * xPercent, PositionScoreA.Y * yPercent),
		colorScoreMyTeam, to_string(scoreA).c_str());
	drawList->AddText(myFont, 110 * xPercent, ImVec2(PositionGametime.X * xPercent, PositionGametime.Y * yPercent),
		colorGameTime, (gameTime).c_str());
	drawList->AddText(myFont, 110 * xPercent, ImVec2(PositionScoreB.X * xPercent, PositionScoreB.Y * yPercent),
		colorScoreOppositeTeam, to_string(scoreB).c_str());*/
}

void CustomUI::drawTextScore(ImDrawList* drawList, Vector2 Position, int fontSize, ImU32 color, SettingsItems settingsAll, SettingsItems settingsItem, string textSettings, string text) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	const auto& sourceSettings = (settingsItems == "settingsScoreAllItems")
		? settingsAll
		: settingsItem;
	const auto& baseSettings = (settingsItems == "settingsScoreAllItems")
		? settingsItem
		: settingsAll;

	const string& stringSettings = (settingsItems == "settingsScoreAllItems")
		? "settingsScoreAllItems"
		: textSettings;


	int vx = Position.X;
	int vy = Position.Y;

	if (allPresets[keyPreset].format == "custom") {
		if (settingsItems != "settingsScoreAllItems") {

			drawList->AddText(myFont, fontSize * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings),
				ImVec2((vx * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings)),
					(vy * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings))),
				color, text.c_str());
		}
		else {
			drawList->AddText(myFont, fontSize * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings),
				ImVec2((vx * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings)),
					(vy * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings))),
				color, text.c_str());
		}
	}
	else {
		drawList->AddText(myFont, fontSize * floatChangeSizeX(settingsAll, "settingsScoreAllItems"),
			ImVec2((vx * floatChangeSizeX(settingsAll, "settingsScoreAllItems") + intChangePositionX(settingsAll, "settingsScoreAllItems")),
				(vy * floatChangeSizeX(settingsAll, "settingsScoreAllItems") + intChangePositionY(settingsAll, "settingsScoreAllItems"))),
			color, text.c_str());
	}
}

void CustomUI::drawBoost(ImDrawList* drawList) {
	
	drawBoostDisplay(drawList);
	if (isTexture) {
		drawBoostTexture(drawList);
		//drawBoostText(drawList, 1660, 825, 1700, 825, 1680, 825);
		drawBoostText(drawList, 140, 120, 180, 120, 160, 120);
	}
	else {
		drawBoostCircle(drawList);
		//drawBoostText(drawList, 1690, 825, 1730, 825, 1710, 825);
		drawBoostText(drawList, 125, 120, 170, 120, 150, 120);
	}

}


void CustomUI::drawBoostDisplay(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	auto& preset = allPresets[keyPreset];
	auto& settingsBoostDisplayArray = preset.settingsBoostDisplay;
	auto& settingsBoostAllItemsArray = preset.settingsBoostAllItems;

	if (!imageDisplayBoost[keyPreset]->IsLoadedForImGui()) {
		return;
	}

	if (auto renderImageBoost = imageDisplayBoost[keyPreset]->GetImGuiTex()) {
		auto size = imageDisplayBoost[keyPreset]->GetSizeF();

		const auto& sourceSettings = (settingsItems == "settingsBoostAllItems")
			? settingsBoostAllItemsArray
			: settingsBoostDisplayArray;
		const auto& baseSettings = (settingsItems == "settingsBoostAllItems")
			? settingsBoostDisplayArray
			: settingsBoostAllItemsArray;

		const string& stringSettings = (settingsItems == "settingsBoostAllItems")
			? "settingsBoostAllItems"
			: "settingsBoostDisplay";

		ImVec2 position = ImVec2(
			0 + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings),
			0 + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings)
		);

		ImVec2 fullSize = ImVec2(
			size.X * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings),
			size.Y * baseSettings.float2 * floatChangeSizeY(sourceSettings, stringSettings)
		);

		if (preset.format == "custom") {
			drawList->AddImage(
				renderImageBoost,
				ImVec2(position.x, position.y),
				ImVec2(position.x + fullSize.x, position.y + fullSize.y),
				ImVec2(0, 0),
				ImVec2(1, 1),
				IM_COL32(255, 255, 255, 255)
			);
		}
		else {
			drawList->AddImage(
				renderImageBoost,
				ImVec2(
					0 + intChangePositionX(settingsBoostAllItemsArray, "settingsBoostAllItems"),
					0 + intChangePositionY(settingsBoostAllItemsArray, "settingsBoostAllItems")
				),
				ImVec2(
					(size.X * floatChangeSizeX(settingsBoostAllItemsArray, "settingsBoostAllItems") +
						intChangePositionX(settingsBoostAllItemsArray, "settingsBoostAllItems")),
					(size.Y * floatChangeSizeY(settingsBoostAllItemsArray, "settingsBoostAllItems") +
						intChangePositionY(settingsBoostAllItemsArray, "settingsBoostAllItems"))
				),
				ImVec2(0, 0),
				ImVec2(1, 1),
				IM_COL32(255, 255, 255, 255)
			);
		}
	}
}

void CustomUI::drawBoostTexture(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	auto& preset = allPresets[keyPreset];
	auto& settingsBoostTexture = preset.settingsBoostTexture;
	auto& settingsBoostAllItems = preset.settingsBoostAllItems;

	if (!imageTextureBoost[keyPreset]->IsLoadedForImGui()) {
		return;
	}

	if (auto renderImageBoost = imageTextureBoost[keyPreset]->GetImGuiTex()) {
		auto size = imageTextureBoost[keyPreset]->GetSizeF();

		// Calcul des ratios et param�tres
		float boostRatio = boost / 100.0f;
		ImVec2 position = { 0, 0 };
		ImVec2 fullSize = { 0, 0 };

		// R�cup�ration des param�tres selon le format
		if (preset.format == "custom") {
			const auto& sourceSettings = (settingsItems == "settingsBoostAllItems")
				? settingsBoostAllItems
				: settingsBoostTexture;
			const auto& baseSettings = (settingsItems == "settingsBoostAllItems")
				? settingsBoostTexture
				: settingsBoostAllItems;

			const string& stringSettings = (settingsItems == "settingsBoostAllItems")
				? "settingsBoostAllItems"
				: "settingsBoostTexture";

			position = ImVec2(
				(80 * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems") + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings)),
				(92 * floatChangeSizeY(settingsBoostAllItems, "settingsBoostAllItems") + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings))
			);
			fullSize = ImVec2(
				size.X * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings),
				size.Y * baseSettings.float2 * floatChangeSizeY(sourceSettings, stringSettings)
			);
		}
		else {
			position = ImVec2(
				(80 * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems") + intChangePositionX(settingsBoostAllItems, "settingsBoostAllItems")),
				(92 * floatChangeSizeY(settingsBoostAllItems, "settingsBoostAllItems") + intChangePositionY(settingsBoostAllItems, "settingsBoostAllItems"))
			);
			fullSize = ImVec2(
				size.X * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems"),
				size.Y * floatChangeSizeY(settingsBoostAllItems, "settingsBoostAllItems")
			);
		}

		// Calcul de la hauteur visible selon le boost (du bas vers le haut)
		float visibleHeight = fullSize.y * boostRatio;

		// Ajuster les UVs pour ne montrer que la hauteur visible
		ImVec2 uv_min = ImVec2(0.0f, 1 - boostRatio); // Bas de l'image en fonction du boost
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);           // Haut de l'image

		// Dessiner l'image
		drawList->AddImage(
			renderImageBoost,
			ImVec2(position.x, position.y + (fullSize.y - visibleHeight)), // Position ajust�e
			ImVec2(position.x + fullSize.x, position.y + fullSize.y),     // Taille ajust�e
			uv_min,
			uv_max,
			IM_COL32(255, 255, 255, 255)
		);
	}
}


void CustomUI::drawBoostCircle(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	ImU32 color = IM_COL32(allPresets[keyPreset].colorBoost[0], allPresets[keyPreset].colorBoost[1], allPresets[keyPreset].colorBoost[2], allPresets[keyPreset].colorBoost[3]);

	ImVec2 center = { 0,0 };
	float radius;
	float strokeThickness;

	auto& preset = allPresets[keyPreset];
	auto& settingsBoostTexture = preset.settingsBoostTexture;
	auto& settingsBoostAllItems = preset.settingsBoostAllItems;

	const auto& sourceSettings = (settingsItems == "settingsBoostAllItems")
		? settingsBoostAllItems
		: settingsBoostTexture;
	const auto& baseSettings = (settingsItems == "settingsBoostAllItems")
		? settingsBoostTexture
		: settingsBoostAllItems;

	const string& stringSettings = (settingsItems == "settingsBoostAllItems")
		? "settingsBoostAllItems"
		: "settingsBoostTexture";

	//center = ImVec2(200 * xPercent, 198 * yPercent); // Centre du cercle
	//radius = 140.0f * xPercent; // Rayon du cercle
	//strokeThickness = 26.0f * xPercent; // �chelle de l'�paisseur du trait

	if (preset.format == "custom") {

		center = ImVec2((200 * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems") + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings)),
			(198 * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems") + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings)));
		radius = 140.0f * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings);
		strokeThickness = 26.0f * baseSettings.float2 * floatChangeSizeX(sourceSettings, stringSettings);

	}
	else {
		center = ImVec2((200 * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems") + intChangePositionX(settingsBoostAllItems, "settingsBoostAllItems")),
			(198 * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems") + intChangePositionY(settingsBoostAllItems, "settingsBoostAllItems")));
		radius = 140.0f * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems");
		strokeThickness = 26.0f * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems");
	}

	// Param�tres pour le cercle

	float startAngle = IM_PI / 2.0f; // D�but de l'arc (� 3h sur un cadran)
	float maxAngle = IM_PI * 2.0f * 0.65f; // 75% du cercle entier
	float endAngle = startAngle + (maxAngle * (boost / 100.0f)); // Calculer l'angle final en fonction du boost

	// Commencer � dessiner le path (cercle)
	drawList->PathClear();
	drawList->PathArcTo(center, radius, startAngle, endAngle, 100); // 100 segments pour le contour lisse
	drawList->PathStroke(color, false, strokeThickness);
}



void CustomUI::drawBoostText(ImDrawList* drawList, int v1x, int v1y, int v2x, int v2y, int v3x, int v3y) {

	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	ImU32 color = IM_COL32(allPresets[keyPreset].colorBoost[0], allPresets[keyPreset].colorBoost[1], allPresets[keyPreset].colorBoost[2], allPresets[keyPreset].colorBoost[3]);

	auto& preset = allPresets[keyPreset];
	auto& settingsBoostText = preset.settingsBoostText;
	auto& settingsBoostAllItems = preset.settingsBoostAllItems;

	const auto& sourceSettings = (settingsItems == "settingsBoostAllItems")
		? settingsBoostAllItems
		: settingsBoostText;
	const auto& baseSettings = (settingsItems == "settingsBoostAllItems")
		? settingsBoostText
		: settingsBoostAllItems;

	const string& stringSettings = (settingsItems == "settingsBoostAllItems")
		? "settingsBoostAllItems"
		: "settingsBoostText";


	int vx = 0, vy = 0;

	if (boost == 100) {
		vx = v1x;
		vy = v1y;
	}
	else if (boost < 10) {
		vx = v2x;
		vy = v2y;
	}
	else {
		vx = v3x;
		vy = v3y;
	}

	if (allPresets[keyPreset].format == "custom") {
		if (settingsItems != "settingsBoostAllItems") {

			drawList->AddText(myFont, 160 * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings),
				ImVec2((vx * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings)),
					(vy * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings))),
				color, to_string(boost).c_str());
		}
		else {
			drawList->AddText(myFont, 160 * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings),
				ImVec2((vx * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings)),
					(vy * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings))),
				color, to_string(boost).c_str());
		}
	}
	else {
		drawList->AddText(myFont, 160 * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems"),
			ImVec2((vx * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems") + intChangePositionX(settingsBoostAllItems, "settingsBoostAllItems")),
				(vy * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems") + intChangePositionY(settingsBoostAllItems, "settingsBoostAllItems"))),
			color, to_string(boost).c_str());
	}


}






// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void CustomUI::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));

}

string CustomUI::GetMenuName(){
	return menuName_;
}

string CustomUI::GetMenuTitle(){
	return menuTitle_;
}
bool CustomUI::ShouldBlockInput(){
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}
bool CustomUI::IsActiveOverlay(){
	return isSettingsOpen;
}	
void CustomUI::OnOpen(){
	
}
void CustomUI::OnClose(){
	if (escape_state)
	{
		escape_state = false;
		isSettingsOpen = false;
		gameWrapper->SetTimeout([&](GameWrapper* gameWrapper) {
			cvarManager->executeCommand("togglemenu " + GetMenuName());
			}, 0.001f);
	}
}