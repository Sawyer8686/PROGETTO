// Copyright Epic Games, Inc. All Rights Reserved.

#include "PROGETTOGameMode.h"
#include "PROGETTOCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DirectionalLight.h"
#include "PROGETTO/Components/DayNightCycleComponent.h"


APROGETTOGameMode::APROGETTOGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

    
        DayNightCycleComponent = CreateDefaultSubobject<UDayNightCycleComponent>(TEXT("DayNightCycleComponent"));
    
}

void APROGETTOGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void APROGETTOGameMode::OnDayStartHandler()
{
    // Aggiungi qui la logica che vuoi eseguire quando inizia il giorno
    
}

void APROGETTOGameMode::OnNightStartHandler()
{
    // Aggiungi qui la logica che vuoi eseguire quando inizia la notte
    
}

void APROGETTOGameMode::BeginPlay()
{
    Super::BeginPlay();


        // Aggiungi il componente per il ciclo giorno-notte
        //DayNightCycleComponent = NewObject<UDayNightCycleComponent>(this);

        //if (DayNightCycleComponent)
        //{
            // Aggiungi il componente al GameMode
            //DayNightCycleComponent->RegisterComponent();

            // Trova la Directional Light nel livello
            ADirectionalLight* SunActor = Cast<ADirectionalLight>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirectionalLight::StaticClass()));
            if (SunActor)
            {
                // Assegna la Directional Light al componente
                DayNightCycleComponent->SunActor = SunActor;
            }
        //}

        // Crea il widget UI
        if (DayNightWidgetClass)
        {
            if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
            {
                DayNightWidget = CreateWidget<UDayNightWidget>(PC, DayNightWidgetClass);
                if (DayNightWidget)
                {
                    DayNightWidget->AddToViewport();
                    DayNightWidget->DayNightComponent = DayNightCycleComponent;
                }
            }
        }

        // Ascolta i delegate
        DayNightCycleComponent->OnDayStart.AddDynamic(this, &APROGETTOGameMode::OnDayStartHandler);
        DayNightCycleComponent->OnNightStart.AddDynamic(this, &APROGETTOGameMode::OnNightStartHandler);
    }




