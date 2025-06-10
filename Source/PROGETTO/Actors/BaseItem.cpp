#include "BaseItem.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PROGETTO/Components/InventoryComponent.h"


// Sets default values
ABaseItem::ABaseItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// Di default abilito la fisica (ma spesso il blueprint può sovrascriverla)
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));


	ItemName = "none";
	Weight = 1.f;
	bStackable = false;
	Quantity = 1;
	MaxStackSize = 1;
	bCanBeConsumed = false;
	HungerRestore = 0.f;
	ThirstRestore = 0.f;
	ItemType = EItemType::Misc;

}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseItem::SetButtonDisabledBlack(UButton* Button)
{
	if (!Button) return;

	// 1) Prendo lo style corrente
	FButtonStyle NewStyle = Button->WidgetStyle;

	// 2) Imposto il tint del ramo "Disabled" a nero opaco
	NewStyle.Disabled.TintColor = FSlateColor(FLinearColor::Black);

	// 3) Applico lo style modificato al bottone
	Button->SetStyle(NewStyle);

	// 4) Disabilito il bottone (adesso userà il DisabledTint = Black)
	Button->SetIsEnabled(false);
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseItem::MainInteract(AActor* Interactor)
{
	// 1) Prendi il PlayerCharacter
	APROGETTOCharacter* Player = Cast<APROGETTOCharacter>(
		UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!Player)
		return;

	// 2) Trova il componente in modo sicuro
	UInventoryComponent* InvComp = Player->FindComponentByClass<UInventoryComponent>();
	if (!InvComp)
		return;

	// 3) Ora chiami AddItemToInventory solo se il componente è valido
	const bool bAdded = InvComp->AddItemToInventory(this);
	if (bAdded)
	{
		// Logica di pickup (nascondi, disattiva collisione, ecc.)
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		if (auto* MeshComp = FindComponentByClass<UStaticMeshComponent>())
		{
			MeshComp->SetVisibility(false);
			MeshComp->SetSimulatePhysics(false);
			MeshComp->SetEnableGravity(false);
		}
	}
}


