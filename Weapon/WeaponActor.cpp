// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "ShooterUE4/ShooterUE4.h"

/*
* Ustawanie domyślnych wartości dla tego aktora
* Można je zmienić w BP
*/
AWeaponActor::AWeaponActor()
{
	// Utworzenie oraz ustalenie meshu broni
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Nazwa lufy broni oraz smugi pocisku
	MuzzleSocket = "MuzzleFlashSocket";
	TracerTargetName = "BulletTrail";

	// Obrażenia od pocisku tej broni
	BaseDamage = 20.0f;

	// Szybkostrzelność
	RateOfFire = 100;

	// Odrzut broni
	BulletSpread = 2.3f;

	//AmmoClip = 4;
	//AmmoPool = 90;

	// Replikacja u innych graczy
	SetReplicates(true);

	// Aktualizacja broni u innych graczy
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

/*
* Funkcja wywoływana przy początku gry, lub przy odrodzeniu
*/
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	// Czas między strzałami
	TimeBetweenShots = 60 / RateOfFire;	
}

/*
* Funkcja wywoływana przy strzelaniu.
* Serwer oblicza gdzie strzelił gracz, jeśli trafił to jest sprawdzana trafiona powierzchnia.
* Na jej podstawie odtwarzany jest odpowiedni efekt.
*/
void AWeaponActor::Fire()
{
	if (AmmoClip == 0) return;

	AmmoClip -= 1;
	//Fire_BP();
	if (!HasAuthority())
	{
		ServerFire();	
	}

	// Ustalenie właściciela broni, a następnie obliczenie trajektorii pocisku biorąc pod uwagę celownik kamery z trzeciej osoby
	AActor* MyOwner = GetOwner();

	/*
	* Jeśli broń nie ma właściciela to jest błąd
	*/
	if (MyOwner == nullptr)
	{
		// Debug
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Weapon Owner is NULL!"));
	}
	if (MyOwner) {

		FVector EyeLocation;
		FRotator EyeRotation;

		// Pobranie lokalizacji postaci gracza, pobranie BulletSpread, oraz ustalenie losowego wektora kierunku strzału.
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		FVector ShotDirection = EyeRotation.Vector();
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		// Ustalenie parametrów względem kamery, ignorowanie postaci gracza
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(ECC_Vehicle);

		// Trace ma brać pod uwagę kompleksową kolizję oraz zwracać informację jaki rodzaj powierzchni został trafiony.
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		// Ustawienie domyślnej powierzchni
		EPhysicalSurface SurfaceType = SurfaceType_Default;
		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;

		/* 
		* Sprawdzenie trafienia pocisku. Jeśli udało się trafić to
		* następnie jest sprawdzany typ powierzchnii. Na podstawie powierzchni 
		* odejmowana jest odpowiednia ilość punktów zdrowia oraz odtwarzany efekt.
		*/
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams)) 
		{
			// Debug
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit!"));

			// Pobranie danych o trafionym aktorze
			AActor* HitActor = Hit.GetActor();

			// Ustalenie trafionej powierzchni
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			// Ustawienie obrażeń
			float ActualDamage = BaseDamage;

			// Jeśli trafiono w głowę, to zwiększ zadane obrażenia
			if (SurfaceType == SURFACE_FLESHVULNERABLE) {
				ActualDamage *= 2.5f;
			}

			// Zadaj trafionemu aktorowi obrażenia
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			// Odtwórz efekt trafienia
			PlayImpactEffect(SurfaceType, Hit.ImpactPoint);

			// Ustalenie miejsca odtworzenia efektu uderzenia pocisku
			TracerEndPoint = Hit.ImpactPoint;

		}

		// Debug linii strzału
		//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

		// Odtwórz efekt strzału
		PlayFireEffects(TracerEndPoint);

		// Serwer dokonuje obliczeń nt. miejsca oraz rodzaju powierzchni
		if (HasAuthority())
		{
			HitScanTrace.TraceTo = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;

		}

		// Zmienna do ognia automatycznego
		LastFire = GetWorld()->TimeSeconds;
	}
	
}

/*
* Odtwarzanie efektów przy strzale
*/
void AWeaponActor::PlayFireEffects(FVector TraceEndPoint)
{
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, MeshComponent, TEXT("MuzzleFlashSocket"));
	// Błysk z lufy broni
	if (MuzzleFlash) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, MeshComponent, TEXT("MuzzleFlashSocket"));
	}

	// Smuga pocisku
	if (TracerEffect) {
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocket);
		UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);

		if (TracerComponent) {
			TracerComponent->SetVectorParameter(TracerTargetName, TraceEndPoint);
		}
	}

	// Trzęsienie kamery jako odrzut
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientStartCameraShake(FireCamShake);
		}
	}
}

/*
* Odtwarzanie efektów przy zderzeniu pocisku z powierzchnią z podziałem na jej rodzaj.
* Rodzaje powierzchni zdefiniowane są w ShooterUE4.h
*/
void AWeaponActor::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType) {
	
	case SURFACE_FLESH:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect) {
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocket);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());

	}
}

/*
* Implementacja strzelania dla wszystkich klientów
*/
void AWeaponActor::ServerFire_Implementation()
{
	Fire();
}

/*
* Walidacja strzału klientów, używana przy opracowywaniu anty-cheatów.
* Tutaj pominięta
*/
bool AWeaponActor::ServerFire_Validate()
{
	return true;
}

/*
* Odtwarzanie efektu strzału oraz uderzenia
*/
void AWeaponActor::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceTo);

	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

/*
* Funkcja wywoływana w każdej klatce
*/ 
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
* Strzał z broni z interwałami, aby nie dało się oszukać automatycznego ognia
*/
void AWeaponActor::StartFire()
{
	float FireDelay = FMath::Max(LastFire + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AWeaponActor::Fire, TimeBetweenShots, true, FireDelay);
}

/*
* Przerwanie strzelania
*/
void AWeaponActor::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

/*
* Funkcja łącząca C++ z BP
*/
void AWeaponActor::Fire_BP_Implementation()
{
}

/*
* Funkcja łącząca C++ z BP
*/
void AWeaponActor::Reload_Implementation()
{
	//AmmoClip = 4;
}


/*
* Replikacja broni u wszystkich graczy
*/
void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeaponActor, HitScanTrace, COND_SkipOwner);
}
