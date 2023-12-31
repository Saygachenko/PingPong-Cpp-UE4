// Fill out your copyright notice in the Description page of Project Settings.


#include "PongBall.h"
#include "Components/BoxComponent.h"
#include "PongPlayer.h"
#include "PongWall.h"
#include "PongGoal.h"

// Sets default values
// //This is where we attach components and customize them.
APongBall::APongBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetEnableGravity(false);
	StaticMeshComponent->SetConstraintMode(EDOFMode::XYPlane);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(StaticMeshComponent);
	BoxComponent->SetCollisionProfileName("Trigger");
	BoxComponent->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

	bGameover = false;
}

// Called when the game starts or when spawned
void APongBall::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);

	if (BallMesh)
	{
		StaticMeshComponent->SetStaticMesh(BallMesh);
	}

	OnActorBeginOverlap.AddDynamic(this, &APongBall::OnPaddleHit);
}

// Ball logic on collision with our PongPlayer.
void APongBall::OnPaddleHit(AActor* OverlappedActor, AActor* OtherActor)
{
	const auto PongPlayer = Cast<APongPlayer>(OtherActor);
	if (PongPlayer )
	{
		const FVector BallLinearVelocity = StaticMeshComponent->GetPhysicsLinearVelocity();
		const FVector PaddleLinearVelocity = PongPlayer->GetStaticMeshComponent()->GetPhysicsLinearVelocity();

		const FVector NewBallLinearVelocity = FVector(BallLinearVelocity.X * -1.0f, PaddleLinearVelocity.Y + FMath::RandRange(-1000.0f, 1000.0f), BallLinearVelocity.Z);

		StaticMeshComponent->SetPhysicsLinearVelocity(NewBallLinearVelocity);
	}

	const auto PongWall = Cast<APongWall>(OtherActor);
	if (PongWall)
	{
		const FVector BallLinearVelocity = StaticMeshComponent->GetPhysicsLinearVelocity();
		const FVector NewBallLinearVelocity = FVector(BallLinearVelocity.X, BallLinearVelocity.Y * -1.0f, BallLinearVelocity.Z);

		StaticMeshComponent->SetPhysicsLinearVelocity(NewBallLinearVelocity);
	}

	const auto PongGoal = Cast<APongGoal>(OtherActor);
	if (PongGoal)
	{
		Restart();
	}
}

// Called every frame
void APongBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Logic of the ball movement at the start of the game.
void APongBall::Start()
{
	bGameover = false;
	SetActorLocation(FVector::ZeroVector);
	StaticMeshComponent->SetPhysicsLinearVelocity(FVector((FMath::RandBool() ? 1.0f : -1.0f) * 2000.0f, FMath::RandRange(-300.0f, 300.0f), 0.0f));
}

// Logic of ball movement when restarting the game.
void APongBall::Restart()
{
	SetActorLocation(FVector::ZeroVector);
	StaticMeshComponent->SetPhysicsLinearVelocity(FVector((FMath::RandBool() ? 1.0f : -1.0f) * 2000.0f, FMath::RandRange(-300.0f, 300.0f), 0.0f));
}

// Logic of ball movement at the end of the game.
void APongBall::GameOver()
{
	bGameover = true;
	StaticMeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

