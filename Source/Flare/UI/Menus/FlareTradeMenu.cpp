
#include "../../Flare.h"
#include "FlareTradeMenu.h"
#include "../../Game/FlareGame.h"
#include "../../Player/FlareMenuManager.h"
#include "../../Player/FlarePlayerController.h"
#include "../Components/FlareRoundButton.h"
#include "../Components/FlareCargoInfo.h"

#define LOCTEXT_NAMESPACE "FlareTradeMenu"


/*----------------------------------------------------
	Construct
----------------------------------------------------*/

void SFlareTradeMenu::Construct(const FArguments& InArgs)
{
	// Data
	MenuManager = InArgs._MenuManager;
	const FFlareStyleCatalog& Theme = FFlareStyleSet::GetDefaultTheme();

	// Build structure
	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		.Padding(Theme.ContentPadding)
		[
			SNew(SHorizontalBox)

			// Icon
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SImage).Image(AFlareMenuManager::GetMenuIcon(EFlareMenu::MENU_Trade))
			]

			// Title
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(Theme.ContentPadding)
			[
				SNew(STextBlock)
				.TextStyle(&Theme.TitleFont)
				.Text(this, &SFlareTradeMenu::GetTitle)
			]

			// Close
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(Theme.TitleButtonPadding)
			.AutoWidth()
			[
				SNew(SFlareRoundButton)
				.Text(LOCTEXT("Back", "Back"))
				.HelpText(LOCTEXT("BackInfo", "Go to the previous menu"))
				.Icon(AFlareMenuManager::GetMenuIcon(EFlareMenu::MENU_Exit, true))
				.OnClicked(this, &SFlareTradeMenu::OnBackClicked)
			]
		]

		// Separator
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(200, 20))
		[
			SNew(SImage).Image(&Theme.SeparatorBrush)
		]

		// Content block
		+ SVerticalBox::Slot()
		.Padding(Theme.ContentPadding)
		.HAlign(HAlign_Center)
		[
			SNew(SScrollBox)
			.Style(&Theme.ScrollBoxStyle)
			.ScrollBarStyle(&Theme.ScrollBarStyle)

			+ SScrollBox::Slot()
			[
				SNew(SHorizontalBox)

				// Left spacecraft aka the current ship
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				[
					SNew(SBox)
					.HAlign(HAlign_Left)
					.WidthOverride(Theme.ContentWidth)
					[
						SNew(SVerticalBox)

						// Current ship's name
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(Theme.TitlePadding)
						[
							SNew(STextBlock)
							.TextStyle(&Theme.SubTitleFont)
							.Text(this, &SFlareTradeMenu::GetLeftSpacecraftName)
						]

						// Current ship's cargo
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(Theme.ContentPadding)
						.HAlign(HAlign_Left)
						[
							SAssignNew(LeftCargoBay, SHorizontalBox)
						]
					]
				]

				// Right spacecraft aka the ship we're going to trade with
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				[
					SNew(SBox)
					.HAlign(HAlign_Fill)
					.WidthOverride(Theme.ContentWidth)
					[
						SNew(SVerticalBox)

						// Ship's name
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(Theme.TitlePadding)
						[
							SNew(STextBlock)
							.TextStyle(&Theme.SubTitleFont)
							.Text(this, &SFlareTradeMenu::GetRightSpacecraftName)
							.Visibility(this, &SFlareTradeMenu::GetTradingVisibility)
						]
				
						// Ship's cargo
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(Theme.ContentPadding)
						.HAlign(HAlign_Left)
						[
							SAssignNew(RightCargoBay, SHorizontalBox)
							.Visibility(this, &SFlareTradeMenu::GetTradingVisibility)
						]

						// Help text
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(Theme.ContentPadding)
						[
							SNew(STextBlock)
							.TextStyle(&Theme.TextFont)
							.Text(LOCTEXT("HelpText", "Click on a resource to start trading."))
							.Visibility(this, &SFlareTradeMenu::GetTradingVisibility)
						]

						// Back button
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(Theme.ContentPadding)
						.HAlign(HAlign_Left)
						[
							SNew(SFlareButton)
							.Text(LOCTEXT("BackToSelection", "Go back to the ship selection"))
							.Icon(FFlareStyleSet::GetIcon("Stop"))
							.OnClicked(this, &SFlareTradeMenu::OnBackToSelection)
							.Visibility(this, &SFlareTradeMenu::GetTradingVisibility)
							.Width(8)
						]
						
						// Ship selection list
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SAssignNew(ShipList, SFlareShipList)
							.MenuManager(MenuManager)
							.Title(LOCTEXT("SelectSpacecraft", "SELECT A SPECACRAFT TO TRADE WITH"))
							.OnItemSelected(this, &SFlareTradeMenu::OnSpacecraftSelected)
						]
					]
				]
			]

			// Price box
			+ SScrollBox::Slot()
			.HAlign(HAlign_Center)
			[
				SNew(SBox)
				.HAlign(HAlign_Fill)
				.WidthOverride(Theme.ContentWidth)
				[
					SNew(SVerticalBox)

					// Title
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(Theme.TitlePadding)
					[
						SNew(STextBlock)
						.TextStyle(&Theme.SubTitleFont)
						.Text(LOCTEXT("TransactionTitle", "TRANSACTION DETAILS"))
						.Visibility(this, &SFlareTradeMenu::GetTransactionDetailsVisibility)
					]

					// Info
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(Theme.ContentPadding)
					[
						SNew(STextBlock)
						.TextStyle(&Theme.TextFont)
						.Text(this, &SFlareTradeMenu::GetTransactionDetails)
						.Visibility(this, &SFlareTradeMenu::GetTransactionDetailsVisibility)
					]

					// Quantity
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(Theme.ContentPadding)
					[
						SAssignNew(QuantitySlider, SSlider)
						.Style(&Theme.SliderStyle)
						.Value(0)
						.OnValueChanged(this, &SFlareTradeMenu::OnResourceQuantityChanged)
						.Visibility(this, &SFlareTradeMenu::GetTransactionDetailsVisibility)
					]

					// Price
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(Theme.ContentPadding)
					[
						SAssignNew(PriceBox, SFlareConfirmationBox)
						.ConfirmText(LOCTEXT("Confirm", "CONFIRM TRANSACTION"))
						.CancelText(LOCTEXT("BackTopShip", "CANCEL"))
						.OnConfirmed(this, &SFlareTradeMenu::OnConfirmTransaction)
						.OnCancelled(this, &SFlareTradeMenu::OnCancelTransaction)
						.FullHide(true)
					]
				]
			]
		]
	];
}


/*----------------------------------------------------
	Interaction
----------------------------------------------------*/

void SFlareTradeMenu::Setup()
{
	SetEnabled(false);
	SetVisibility(EVisibility::Hidden);
	ShipList->SetVisibility(EVisibility::Collapsed);
}

void SFlareTradeMenu::Enter(UFlareSimulatedSector* ParentSector, UFlareSimulatedSpacecraft* LeftSpacecraft, UFlareSimulatedSpacecraft* RightSpacecraft)
{
	FLOGV("SFlareTradeMenu::Enter ParentSector=%p LeftSpacecraft=%p RightSpacecraft=%p", ParentSector, LeftSpacecraft, RightSpacecraft);

	SetEnabled(true);
	SetVisibility(EVisibility::Visible);

	// Setup targets
	TargetSector = ParentSector;
	TargetLeftSpacecraft = LeftSpacecraft;
	TargetRightSpacecraft = RightSpacecraft;

	// Setup menus
	AFlarePlayerController* PC = MenuManager->GetPC();
	FillTradeBlock(TargetLeftSpacecraft, TargetRightSpacecraft, LeftCargoBay);
	FillTradeBlock(TargetRightSpacecraft, TargetLeftSpacecraft, RightCargoBay);
	
	// Add stations
	for (int32 SpacecraftIndex = 0; SpacecraftIndex < ParentSector->GetSectorStations().Num(); SpacecraftIndex++)
	{
		UFlareSimulatedSpacecraft* StationCandidate = ParentSector->GetSectorStations()[SpacecraftIndex];
		if (StationCandidate && StationCandidate != LeftSpacecraft && StationCandidate != RightSpacecraft
		 && StationCandidate->GetDescription()->CargoBayCount > 0)
		{
			ShipList->AddShip(StationCandidate);
		}
	}

	// Add ships
	for (int32 SpacecraftIndex = 0; SpacecraftIndex < ParentSector->GetSectorShips().Num(); SpacecraftIndex++)
	{
		UFlareSimulatedSpacecraft* ShipCandidate = ParentSector->GetSectorShips()[SpacecraftIndex];
		if (ShipCandidate && ShipCandidate != LeftSpacecraft && ShipCandidate != RightSpacecraft
		 && ShipCandidate->GetDescription()->CargoBayCount > 0
		 && ShipCandidate->GetDamageSystem()->IsAlive())
		{
			ShipList->AddShip(ShipCandidate);
		}
	}

	// Setup widgets
	ShipList->RefreshList();
	ShipList->SetVisibility(EVisibility::Visible);
}

void SFlareTradeMenu::FillTradeBlock(UFlareSimulatedSpacecraft* TargetSpacecraft, UFlareSimulatedSpacecraft* OtherSpacecraft, TSharedPtr<SHorizontalBox> CargoBay)
{
	const FFlareStyleCatalog& Theme = FFlareStyleSet::GetDefaultTheme();
	CargoBay->ClearChildren();

	// Both spacecrafts are set
	if (TargetSpacecraft)
	{
		TArray<FFlareCargo>& SpacecraftCargoBay = TargetSpacecraft->GetCargoBay();
		for (int CargoIndex = 0; CargoIndex < SpacecraftCargoBay.Num(); CargoIndex++)
		{
			FFlareCargo* Cargo = &SpacecraftCargoBay[CargoIndex];
			CargoBay->AddSlot()
			[
				SNew(SFlareCargoInfo)
				.Spacecraft(TargetSpacecraft)
				.CargoIndex(CargoIndex)
				.OnClicked(this, &SFlareTradeMenu::OnTransferResources, TargetSpacecraft, OtherSpacecraft, Cargo->Resource)
			];
		}
	}
}

void SFlareTradeMenu::Exit()
{
	SetEnabled(false);

	// Reset cargo
	TargetLeftSpacecraft = NULL;
	TargetRightSpacecraft = NULL;
	LeftCargoBay->ClearChildren();
	RightCargoBay->ClearChildren();

	// Reset transaction data
	TransactionDestinationSpacecraft = NULL;
	TransactionSourceSpacecraft = NULL;
	TransactionResource = NULL;

	// Reset menus
	PriceBox->Hide();
	ShipList->Reset();
	SetVisibility(EVisibility::Hidden);
}


/*----------------------------------------------------
	Callbacks
----------------------------------------------------*/

EVisibility SFlareTradeMenu::GetTradingVisibility() const
{
	return (TargetLeftSpacecraft && TargetRightSpacecraft) ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SFlareTradeMenu::GetTransactionDetailsVisibility() const
{
	return (TransactionSourceSpacecraft && TransactionDestinationSpacecraft && TransactionResource) ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SFlareTradeMenu::GetTitle() const
{
	return LOCTEXT("Trade", "TRADE");
}

FText SFlareTradeMenu::GetLeftSpacecraftName() const
{
	if (TargetLeftSpacecraft)
	{
		return FText::FromName(TargetLeftSpacecraft->GetImmatriculation());
	}
	else
	{
		return FText();
	}
}

FText SFlareTradeMenu::GetRightSpacecraftName() const
{
	if (TargetRightSpacecraft)
	{
		return FText::FromName(TargetRightSpacecraft->GetImmatriculation());
	}
	else
	{
		return LOCTEXT("NoSelectedSpacecraft", "No spacecraft selected");
	}
}

FText SFlareTradeMenu::GetTransactionDetails() const
{
	if (TransactionSourceSpacecraft && TransactionDestinationSpacecraft && TransactionResource)
	{
		return FText::Format(LOCTEXT("TradeInfoFormat", "Trading {0}x {1} from {2} to {3}"),
			FText::AsNumber(TransactionQuantity),
			TransactionResource->Name,
			FText::FromName(TransactionSourceSpacecraft->GetImmatriculation()),
			FText::FromName(TransactionDestinationSpacecraft->GetImmatriculation()));
	}
	else
	{
		return FText();
	}
}

void SFlareTradeMenu::OnBackClicked()
{
	MenuManager->Back();
}

void SFlareTradeMenu::OnSpacecraftSelected(TSharedPtr<FInterfaceContainer> SpacecraftContainer)
{
	UFlareSimulatedSpacecraft* Spacecraft = Cast<UFlareSimulatedSpacecraft>(SpacecraftContainer->ShipInterfacePtr);

	if (Spacecraft)
	{
		// Store spacecrafts
		if (TargetLeftSpacecraft)
		{
			TargetRightSpacecraft = Spacecraft;
		}
		else
		{
			TargetLeftSpacecraft = Spacecraft;
		}

		// Reset menus
		PriceBox->Hide();
		FillTradeBlock(TargetRightSpacecraft, TargetLeftSpacecraft, RightCargoBay);
		FillTradeBlock(TargetLeftSpacecraft, TargetRightSpacecraft, LeftCargoBay);
		ShipList->SetVisibility(EVisibility::Collapsed);
	}
}

void SFlareTradeMenu::OnTransferResources(UFlareSimulatedSpacecraft* SourceSpacecraft, UFlareSimulatedSpacecraft* DestinationSpacecraft, FFlareResourceDescription* Resource)
{
	if (DestinationSpacecraft)
	{
		// Store transaction data
		TransactionSourceSpacecraft = SourceSpacecraft;
		TransactionDestinationSpacecraft = DestinationSpacecraft;
		TransactionResource = Resource;
		TransactionQuantity = TransactionSourceSpacecraft->GetCargoBayResourceQuantity(TransactionResource);
		QuantitySlider->SetValue(1.0f);

		// Update price (TODO)
		uint32 ResourceUnitPrice = 42;//SourceSpacecraft->GetSellingPrice(TransactionResource);
		if (TransactionSourceSpacecraft && ResourceUnitPrice > 0)
		{
			PriceBox->Show(TransactionQuantity * ResourceUnitPrice);
		}
	}
}

void SFlareTradeMenu::OnResourceQuantityChanged(float Value)
{
	// Force slider value, update quantity
	int32 ResourceMaxQuantity = FMath::Min(TransactionSourceSpacecraft->GetCargoBayResourceQuantity(TransactionResource), TransactionSourceSpacecraft->GetDescription()->CargoBayCapacity);
	TransactionQuantity = FMath::Lerp((int32)1, ResourceMaxQuantity, Value);
	QuantitySlider->SetValue((float)(TransactionQuantity - 1) / (float)(ResourceMaxQuantity - 1));
	FLOGV("SFlareTradeMenu::OnResourceQuantityChanged %f -> %d/%d", Value, TransactionQuantity, ResourceMaxQuantity);

	// Update price (TODO)
	uint32 ResourceUnitPrice = 42;//SourceSpacecraft->GetSellingPrice(TransactionResource);
	if (TransactionSourceSpacecraft && ResourceUnitPrice > 0)
	{
		PriceBox->Show(TransactionQuantity * ResourceUnitPrice);
	}
}

void SFlareTradeMenu::OnConfirmTransaction()
{
	// Actual transaction
	if (TransactionSourceSpacecraft && TransactionDestinationSpacecraft && TransactionResource)
	{
		TargetSector->GetGame()->GetGameWorld()->TransfertResources(TransactionSourceSpacecraft, TransactionDestinationSpacecraft, TransactionResource, TransactionQuantity);
	}

	// Reset transaction data
	TransactionDestinationSpacecraft = NULL;
	TransactionSourceSpacecraft = NULL;
	TransactionResource = NULL;
	TransactionQuantity = 0;

	// Reset menus
	PriceBox->Hide();
	FillTradeBlock(TargetRightSpacecraft, TargetLeftSpacecraft, RightCargoBay);
	FillTradeBlock(TargetLeftSpacecraft, TargetRightSpacecraft, LeftCargoBay);
}

void SFlareTradeMenu::OnCancelTransaction()
{
	// Reset transaction data
	TransactionDestinationSpacecraft = NULL;
	TransactionSourceSpacecraft = NULL;
	TransactionResource = NULL;
	TransactionQuantity = 0;

	// Reset menus
	PriceBox->Hide();
	FillTradeBlock(TargetRightSpacecraft, TargetLeftSpacecraft, RightCargoBay);
	FillTradeBlock(TargetLeftSpacecraft, TargetRightSpacecraft, LeftCargoBay);
}

void SFlareTradeMenu::OnBackToSelection()
{
	TargetRightSpacecraft = NULL;

	// Reset transaction data
	TransactionDestinationSpacecraft = NULL;
	TransactionSourceSpacecraft = NULL;
	TransactionResource = NULL;
	TransactionQuantity = 0;

	// Reset menus
	PriceBox->Hide();
	RightCargoBay->ClearChildren();
	ShipList->ClearSelection();
	ShipList->SetVisibility(EVisibility::Visible);
}

#undef LOCTEXT_NAMESPACE

