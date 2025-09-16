#include "RHIHandler.hpp"
#include "RHIResource.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine::Platform;

TEST(RHIHandlerTest, Shared)
{
    RHIHandler<RHIResource> handler1(new RHIResource());
    EXPECT_EQ(handler1->GetRefCount(), 1);

    {
        RHIHandler<RHIResource> handler2 = handler1;
        EXPECT_EQ(handler1->GetRefCount(), 2);
        EXPECT_EQ(handler2->GetRefCount(), 2);
    }
    EXPECT_EQ(handler1->GetRefCount(), 1);
}
TEST(RHIHandlerTest, Move)
{
    RHIHandler<RHIResource> handler1(new RHIResource());
    EXPECT_EQ(handler1->GetRefCount(), 1);

    {
        RHIHandler<RHIResource> handler2 = std::move(handler1);
        EXPECT_EQ(handler2->GetRefCount(), 1);
        EXPECT_EQ(handler1, nullptr);
    }
}
TEST(RHIHandlerTest, PendingDelete)
{
    PendingDeletes.Clear();
    RHIHandler<RHIResource> handler1(new RHIResource());
    EXPECT_EQ(handler1->GetRefCount(), 1);
    handler1->AddRef();
    EXPECT_EQ(handler1->GetRefCount(), 2);
    handler1->Release();
    EXPECT_EQ(handler1->GetRefCount(), 1);
    EXPECT_EQ(PendingDeletes.Size(), 0);
    handler1->Release();
    EXPECT_EQ(handler1->GetRefCount(), 0);
    EXPECT_EQ(PendingDeletes.Size(), 1);
    auto resource = PendingDeletes.Consume();
    EXPECT_EQ(PendingDeletes.Size(), 0);
}