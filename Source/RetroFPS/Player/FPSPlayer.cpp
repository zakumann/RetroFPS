// Fill out your copyright notice in the Description page of Project Settings.


#include "RetroFPS/Player/FPSPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

//Door
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
AFPSPlayer::AFPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// Create a CameraComponent 
	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCamera->SetupAttachment(GetCapsuleComponent());
	FPSCamera->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	FPSCamera->bUsePawnControlRotation = true;

	// Set default values
	WalkSpeed = 600.0f;
	SprintSpeed = 1200.0f;

}


// Called when the game starts or when spawned
void AFPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
}

// Called every frame
void AFPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// MoveForward
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AFPSPlayer::MoveForward);
		// MoveBackward
		EnhancedInputComponent->BindAction(MoveBackwardAction, ETriggerEvent::Triggered, this, &AFPSPlayer::MoveBackward);
		// MoveRight
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AFPSPlayer::MoveRight);
		// MoveLeft
		EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Triggered, this, &AFPSPlayer::MoveLeft);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSPlayer::Look);

		//Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AFPSPlayer::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AFPSPlayer::StopSprint);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFPSPlayer::Interact);
	}
}

void AFPSPlayer::MoveForward(const FInputActionValue& InputValue)
{
	FVector2D MovementVector = InputValue.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		
	}
}

void AFPSPlayer::MoveBackward(const FInputActionValue& InputValue)
{
	FVector2D MovementVector = InputValue.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement
		AddMovementInput(GetActorForwardVector(), -MovementVector.Y);

	}
}

void AFPSPlayer::MoveRight(const FInputActionValue& InputValue)
{
	FVector2D MovementVector = InputValue.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFPSPlayer::MoveLeft(const FInputActionValue& InputValue)
{
	FVector2D MovementVector = InputValue.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorRightVector(), -MovementVector.X);
	}
}

void AFPSPlayer::Look(const FInputActionValue& InputValue)
{
	FVector2D LookAxisVector = InputValue.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFPSPlayer::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact key pressed!"));

	FHitResult HitResult;
	FVector Start = FPSCamera->GetComponentLocation();
	FVector End = Start + FPSCamera->GetForwardVector() * InteractLineTraceLength;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);
	DrawDebugPoint(GetWorld(), End, 20.f, FColor::Red, false, 2.f);
	DrawDebugPoint(GetWorld(), Start, 20.f, FColor::Blue, false, 2.f);

	ADoor* Door = Cast<ADoor>(HitResult.GetActor());
	if (Door)
	{
		Door->Character = this;
		Door->OnInteract();
	}
}

void AFPSPlayer::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AFPSPlayer::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
