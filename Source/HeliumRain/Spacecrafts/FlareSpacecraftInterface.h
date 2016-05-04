#pragma once

#include "FlareSpacecraftTypes.h"
#include "FlareShipPilot.h"
#include "FlareSpacecraftComponent.h"
#include "Subsystems/FlareSpacecraftDamageSystem.h"
#include "Subsystems/FlareSpacecraftNavigationSystem.h"
#include "Subsystems/FlareSpacecraftDockingSystem.h"
#include "Subsystems/FlareSpacecraftWeaponsSystem.h"
#include "../Data/FlareFactoryCatalogEntry.h"
#include "FlareSpacecraftInterface.generated.h"

class UFlareCompany;
class IFlareSpacecraftDamageSystemInterface;
class IFlareSpacecraftNavigationSystemInterface;
class IFlareSpacecraftWeaponsSystemInterface;
class IFlareSpacecraftDockingSystemInterface;
class UFlareCargoBay;


/** Catalog binding between FFlareSpacecraftDescription and FFlareSpacecraftComponentDescription structure */
USTRUCT()
struct FFlareSpacecraftSlotDescription
{
	GENERATED_USTRUCT_BODY()

	/** Slot internal name */
	UPROPERTY(EditAnywhere, Category = Content) FName SlotIdentifier;

	/** Component description can be empty if configurable slot */
	UPROPERTY(EditAnywhere, Category = Content) FName ComponentIdentifier;

	/** Component slot name */
	UPROPERTY(EditAnywhere, Category = Content) FText SlotName;

	/** Size of the slot  */
	UPROPERTY(EditAnywhere, Category = Content)
	TEnumAsByte<EFlarePartSize::Type> Size;

	/** Turret angle limits. The number of value indicate indicate the angular between each limit. For exemple 4 value are for 0°, 90°, -90° and 180°? */
	UPROPERTY(EditAnywhere, Category = Content)
	TArray<float> TurretBarrelsAngleLimit;

	/** Power components */
	UPROPERTY(EditAnywhere, Category = Content)
	TArray<FName> PoweredComponents;
};

USTRUCT()
struct FFlareSpacecraftDynamicComponentStateDescription
{
	GENERATED_USTRUCT_BODY()

	/** Dynamic component state name */
	UPROPERTY(EditAnywhere, Category = Content) FName StateIdentifier;

	/** Dynamic component state templates */
	UPROPERTY(EditAnywhere, Category = Content)
	TArray<UBlueprint*> StateTemplates;

};

/** Spacecraft capabilities */
UENUM()
namespace EFlareSpacecraftCapability
{
	enum Type
	{
		Consumer,
		Storage,
		Maintenance
	};
}

/** Build constraints for stations */
UENUM()
namespace EFlareBuildConstraint
{
	enum Type
	{
		FreeAsteroid,
		SunExposure,
		HideOnIce,
		HideOnNoIce,
		GeostationaryOrbit
	};
}

/** Catalog data structure for a spacecraft */
USTRUCT()
struct FFlareSpacecraftDescription
{
	GENERATED_USTRUCT_BODY()

	/** Spacecraft internal name */
	UPROPERTY(EditAnywhere, Category = Content) FName Identifier;

	/** Spacecraft name */
	UPROPERTY(EditAnywhere, Category = Content) FText Name;

	/** Spacecraft description */
	UPROPERTY(EditAnywhere, Category = Content) FText Description;

	/** Spacecraft description */
	UPROPERTY(EditAnywhere, Category = Content) FText ImmatriculationCode;

	/** Spacecraft mass */
	UPROPERTY(EditAnywhere, Category = Content) float Mass;

	/** Build constraints for stations */
	UPROPERTY(EditAnywhere, Category = Content)
	TArray<TEnumAsByte<EFlareBuildConstraint::Type>> BuildConstraint;

	/** Size of the ship components */
	UPROPERTY(EditAnywhere, Category = Save)
	TEnumAsByte<EFlarePartSize::Type> Size;

	/** Number of RCS */
	UPROPERTY(EditAnywhere, Category = Save) int32 RCSCount;

	/** Number of orbital engine */
	UPROPERTY(EditAnywhere, Category = Save) int32 OrbitalEngineCount;

	/** Gun slots */
	UPROPERTY(EditAnywhere, Category = Content)
	TArray<FFlareSpacecraftSlotDescription> GunSlots;

	/** Turret slots */
	UPROPERTY(EditAnywhere, Category = Content)
	TArray<FFlareSpacecraftSlotDescription> TurretSlots;

	/** Internal component slots */
	UPROPERTY(EditAnywhere, Category = Content)
	TArray<FFlareSpacecraftSlotDescription> InternalComponentSlots;

	/** Dynamic component states */
	UPROPERTY(EditAnywhere, Category = Content)
	TArray<FFlareSpacecraftDynamicComponentStateDescription> DynamicComponentStates;

	/** Max angular velocity in degree/s */
	UPROPERTY(EditAnywhere, Category = Content)	float AngularMaxVelocity;

	/** Max linear velocity in m/s */
	UPROPERTY(EditAnywhere, Category = Content)	float LinearMaxVelocity;

	/** Heat capacity un KJ/K */
	UPROPERTY(EditAnywhere, Category = Content) float HeatCapacity;

	/** Spacecraft mesh name */
	UPROPERTY(EditAnywhere, Category = Content) UBlueprint* Template;

	/** Spacecraft mesh preview image */
	UPROPERTY(EditAnywhere, Category = Content) FSlateBrush MeshPreviewBrush;

	/** Engine Power sound*/
	UPROPERTY(EditAnywhere, Category = Content) USoundCue* PowerSound;

	/** Cargo bay count.*/
	UPROPERTY(EditAnywhere, Category = Content)
	uint32 CargoBayCount;

	/** Cargo bay capacity.*/
	UPROPERTY(EditAnywhere, Category = Content)
	uint32 CargoBayCapacity;

	/** Factories*/
	UPROPERTY(EditAnywhere, Category = Save)
	TArray<UFlareFactoryCatalogEntry*> Factories;

	/** Is people can consume resource in this station */
	UPROPERTY(EditAnywhere, Category = Content)
	TArray<TEnumAsByte<EFlareSpacecraftCapability::Type>> Capabilities;

	/** Cycle cost & yields */
	UPROPERTY(EditAnywhere, Category = Content)
	FFlareProductionData CycleCost;
};

/** Interface wrapper */
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UFlareSpacecraftInterface  : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


/** Actual interface */
class IFlareSpacecraftInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	/*----------------------------------------------------
		Save
	----------------------------------------------------*/

	/** Load the ship from a save file */
	virtual void Load(const FFlareSpacecraftSave& Data) = 0;

	/** Save the ship to a save file */
	virtual FFlareSpacecraftSave* Save() = 0;

	virtual AFlareGame* GetGame() const = 0;

	/** Get the parent company */
	virtual UFlareCompany* GetCompany() = 0;

	/** Get the ship nick name */
	virtual FText GetNickName() const = 0;

	/** Get the ship description*/
	virtual FFlareSpacecraftDescription* GetDescription() const = 0;

	/** Get the ship size class */
	virtual EFlarePartSize::Type GetSize() = 0;

	virtual FName GetImmatriculation() const = 0;

	/** Check if this is a military ship */
	virtual bool IsMilitary() const = 0;

	/** Check if this is a station ship */
	virtual bool IsStation() const = 0;

	virtual bool CanBeFlown(FText& OutInfo) const = 0;

	virtual bool CanTravel() const = 0;

	virtual bool CanFight() const = 0;

	virtual bool IsAssignedToSector() const = 0;

	virtual void AssignToSector(bool Assign) = 0;

	virtual UFlareCargoBay* GetCargoBay() = 0;

	virtual UFlareSectorInterface* GetCurrentSectorInterface() = 0;


	/*----------------------------------------------------
		Resources
	----------------------------------------------------*/

	/** Is this resource used as input by this station ?*/
	virtual bool IsInputResource(FFlareResourceDescription* Resource);

	/** Is this resource used as output by this station ?*/
	virtual bool IsOutputResource(FFlareResourceDescription* Resource);


	/*----------------------------------------------------
		Sub system
	----------------------------------------------------*/

	virtual IFlareSpacecraftDamageSystemInterface* GetDamageSystem() const = 0;

	virtual IFlareSpacecraftNavigationSystemInterface* GetNavigationSystem() const = 0;

	virtual IFlareSpacecraftDockingSystemInterface* GetDockingSystem() const = 0;

	virtual IFlareSpacecraftWeaponsSystemInterface* GetWeaponsSystem() const = 0;

	/*----------------------------------------------------
		Content
	----------------------------------------------------*/

	/** Get a Slate brush */
	static const FSlateBrush* GetIcon(FFlareSpacecraftDescription* Characteristic);

	static bool IsStation(FFlareSpacecraftDescription* SpacecraftDesc);

	static bool IsMilitary(FFlareSpacecraftDescription* SpacecraftDesc);
};
