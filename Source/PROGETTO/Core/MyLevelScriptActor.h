// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "PROGETTO/Components/DayNightCycleComponent.h"
#include "MyLevelScriptActor.generated.h"

class UAudioComponent;
class USoundCue;


UCLASS()
class PROGETTO_API AMyLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
    AMyLevelScriptActor();

    // Riferimenti per la musica del giorno e della notte
    UPROPERTY(EditAnywhere, Category = "Music")
    USoundCue* DayMusicCue;

    UPROPERTY(EditAnywhere, Category = "DayNightCycle")
    UDayNightCycleComponent* DayNightCycleComponent;

    UPROPERTY(EditAnywhere, Category = "Music")
    USoundCue* NightMusicCue;

    // Componente Audio per la musica
    UAudioComponent* MusicAudioComponent;

protected:
    virtual void BeginPlay() override;

private:
    // Funzioni chiamate dai delegate del ciclo giorno/notte
    UFUNCTION()
    void OnNightStartHandler();

    UFUNCTION()
    void OnDayStartHandler();
};
	

