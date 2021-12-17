// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "WeaponActor.h"
#include "ShooterUE4/ShooterUE4.h"
#include "HealthComponent.h"
#include "GameModePlot.h"

/*
* Funkcja wywoływana przy początku gry, lub przy odrodzeniu
*/
AMainCharacter::AMainCharacter()
{
	// Czy funkcja Tick ma działać
	PrimaryActorTick.bCanEverTick = true;

	// Tworzenie i przydzielenie SpringArm do postaci
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	// Tworzenie i przydzielenie kamery do SpringArm
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	// Czy postać może kucać
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Wartość prawego przybliżenia oraz szybkość
	ZoomFOV = 65.00f;
	ZoomInterpSpeed = 20;

	// Nazwa socketu, do którego zostanie dołączona broń
	WeaponAttachSocketName = "WeaponSocketR";

	// Broń nie otrzymuje żadnych obrażeń
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	// Zdrowie dla postaci
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

}

/*
* Funkcja wywoływana przy początku gry, lub przy odrodzeniu
*/
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Ustawienie domyślnej pozycji kamery
	DefaultFOV = CameraComponent->FieldOfView;

	// Przy zmianie stanu zdrowia gracza, odwołanie do funkcji OnHealthChanged
	HealthComponent->OnHealthChanged.AddDynamic(this, &AMainCharacter::OnHealthChanged);


	if (HasAuthority()) {

		// Ukrycie oryginalnej broni dołączonej do modelu
		GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);

		// Utworzenie parametrów broni
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Spawn broni dla gracza
		Weapon = GetWorld()->SpawnActor <AWeaponActor>(StarterWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		// Jeśli udało się utworzyć broń to przydziel ją graczowi, oraz połącz ją z modelem
		if (Weapon) {
			Weapon->SetOwner(this);
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}
}

/*
* Funkcja wywoływana w każdej klatce
*/
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Wartość przybliżenia kamera, czy przyciśnięty jest prawy klawisz, ustaw według gracza
	float TargetFOV = bZoom ? ZoomFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	CameraComponent->SetFieldOfView(NewFOV);
}

/*
* Bindowanie poruszania się gracza
*/
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Poruszanie się do przodu, w tył oraz na boki
	PlayerInputComponent->BindAxis("Forward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Side", this, &AMainCharacter::MoveRight);

	// Kierowanie myszką
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::AddControllerYawInput);

	// Kucanie
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMainCharacter::EndCrouch);

	// Zoom przy przytrzymywaniu prawego przycisku myszy
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AMainCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AMainCharacter::EndZoom);

	// Strzelanie lewym przyciskiem myszy
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMainCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMainCharacter::StopFire);

	// Skakanie
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	// Przeładowanie
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMainCharacter::Reload);
}


/*
* Poruszanie się do przodu oraz w tył
*/
void AMainCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

/*
* Poruszanie na boki
*/
void AMainCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

/*
* Kucanie
*/
void AMainCharacter::BeginCrouch_Implementation()
{
	Crouch();
}


void AMainCharacter::EndCrouch_Implementation()
{
	UnCrouch();
}

/*
* Zoom na prawym przycisku myszy
*/
void AMainCharacter::BeginZoom()
{
	bZoom = true;
}

/*
* Przywrócenie kamery na poprzednią pozycję
*/
void AMainCharacter::EndZoom()
{
	bZoom = false;
}

/*
* Funkcja łącząca C++ z BP
*/
void AMainCharacter::CharacterFire_Implementation()
{
}

/*
* Funkcja łącząca C++ z BP
*/
void AMainCharacter::ClearWeapons_Implementation()
{
}

/*
* Przeładowanie
*/
void AMainCharacter::Reload()
{
	if (Weapon) {
		Weapon->Reload();
	}
}

/*
* Funkcja wykorzystywana w BTTask_Shoot
* Umożliwia AI strzelania wykorzystując C++
*/
void AMainCharacter::ShootAI()
{
	if (Weapon) {
		Weapon->Fire();
	}
}

/*
* Rozpoczęcie strzelania
*/
void AMainCharacter::StartFire()
{
	if (Weapon) {
		CharacterFire();
		Weapon->StartFire();
	}
}

/*
* Zakończenie strzelania
*/
void AMainCharacter::StopFire()
{
	if (Weapon)
	{
		Weapon->StopFire();
	}
}

/*
* Zmiana zdrowia przy otrzymaniu obrażeń, ew. śmierć postaci
*/
void AMainCharacter::OnHealthChanged(UHealthComponent* OHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// Jeśli gracz ma <=0 zdrowia, to zablokuj poruszanie się, usuń broń, pozwól na chodzenie po nim, usuń model po 5 sekundach
	if (Health <= 0.0f && !bDied) {

		AGameModePlot* GameMode = GetWorld()->GetAuthGameMode<AGameModePlot>();
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}

		bDied = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(5.0f);
		ClearWeapons();
		//Weapon->Destroy();//DODANA LINIJKA, CZY NIE ZESPUJE PRZY ODRADZANIU? || Tak psuje ostro, naprawic albo wywalic || Naprawione w BP

		
	}
}

/*
*  Zwraca lokalizację kamery
*/
FVector AMainCharacter::GetPawnViewLocation() const
{
	if (CameraComponent)
	{
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

/*
* Replikuj broń i śmierć danego gracza u wszystkich
*/
void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainCharacter, Weapon);
	DOREPLIFETIME(AMainCharacter, bDied);
}