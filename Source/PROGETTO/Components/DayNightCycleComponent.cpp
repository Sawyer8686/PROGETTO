#include "DayNightCycleComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"


UDayNightCycleComponent::UDayNightCycleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UDayNightCycleComponent::BeginPlay()
{
	Super::BeginPlay();

	bIsDay = true;  
	CurrentTimeOfDay = 0.f;  
	
}

void UDayNightCycleComponent::UpdateSunPosition()
{
	if (SunActor)
	{
		FRotator NewRotation = FRotator((CurrentTimeOfDay * 360.f), 0.f, 0.f);
		SunActor->SetActorRotation(NewRotation);
		float PitchRotation = CurrentTimeOfDay * 360.0f;
	}

}

void UDayNightCycleComponent::CheckDayNightTransition()
{
	float TimeOfDay = CurrentTimeOfDay;

	bool bIsCurrentlyDay = (CurrentTimeOfDay > 0.5f); 

	if (bIsCurrentlyDay && !bIsDay)
	{
		bIsDay = true;
		OnDayStart.Broadcast();  
	}
	else if (!bIsCurrentlyDay && bIsDay)
	{
		bIsDay = false;
		OnNightStart.Broadcast();  
	}

}


void UDayNightCycleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentTimeOfDay -= (DeltaTime / DayLengthInSeconds) * TimeSpeed;
	if (CurrentTimeOfDay < 0.f)
	{
		CurrentTimeOfDay = 1.f;  
	}

	UpdateSunPosition();
	CheckDayNightTransition();

}
