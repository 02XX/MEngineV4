#include "RHIHandler.hpp"
#include "RHIResource.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine::Platform;

TEST(RHIHandlerTest, Shared)
{
    RHIHandler<RHIResource> handler1(new RHIResource());
    EXPECT_EQ(handler1.GetRefCount(), 1);
    auto handler2(handler1);
    EXPECT_EQ(handler1.GetRefCount(), 2);
    handler2.Release();
    EXPECT_EQ(handler1.GetRefCount(), 1);
    handler1.Release();
    EXPECT_EQ(handler1.GetRefCount(), 0);
}
TEST(RHIHandlerTest, Move)
{
    RHIHandler<RHIResource> handler1(new RHIResource());
    EXPECT_EQ(handler1.GetRefCount(), 1);
    auto handler2 = std::move(handler1);
    EXPECT_EQ(handler2.GetRefCount(), 1);
    EXPECT_EQ(handler1.GetRefCount(), 0);
    handler2.Release();
    EXPECT_EQ(handler2.GetRefCount(), 0);
}
TEST(SHAREDPTRTEST, Default)
{
    auto ptr = std::shared_ptr<int>(new int(42));
    EXPECT_EQ(ptr.use_count(), 1);
    auto ptr2 = ptr;
    ptr.reset();
    EXPECT_EQ(ptr.use_count(), 0);
    EXPECT_EQ(ptr2.use_count(), 1);
}
TEST(RHIHandlerTest, Assign)
{
    RHIHandler<RHIResource> handler1(new RHIResource());
    RHIHandler<RHIResource> handler2;
    EXPECT_EQ(handler1.GetRefCount(), 1);
    EXPECT_EQ(handler2.GetRefCount(), 0);
    handler2 = handler1;
    EXPECT_EQ(handler1.GetRefCount(), 2);
    EXPECT_EQ(handler2.GetRefCount(), 2);
    handler1.Release();
    EXPECT_EQ(handler1.GetRefCount(), 0);
    EXPECT_EQ(handler2.GetRefCount(), 1);
    handler2.Release();
    EXPECT_EQ(handler2.GetRefCount(), 0);
}
TEST(RHIHandlerTest, Delete)
{
    PendingDeletes = MPMCQueue<RHIResource *>();
    auto handler1 = RHIHandler<RHIResource>(new RHIResource());
    EXPECT_EQ(handler1.GetRefCount(), 1);
    EXPECT_EQ(PendingDeletes.Size(), 0);
    handler1.Release();
    EXPECT_EQ(handler1.GetRefCount(), 0);
    EXPECT_EQ(PendingDeletes.Size(), 1);
    auto deletedResource = PendingDeletes.Consume();
    EXPECT_NE(deletedResource, nullptr);
    EXPECT_EQ(PendingDeletes.Size(), 0);
}
TEST(RHIHandlerTest, MultiDelete)
{
    PendingDeletes = MPMCQueue<RHIResource *>();
    auto handler1 = RHIHandler<RHIResource>(new RHIResource());
    auto handler2 = handler1;
    auto handler3 = handler2;
    EXPECT_EQ(handler1.GetRefCount(), 3);
    EXPECT_EQ(PendingDeletes.Size(), 0);
    handler1.Release();
    EXPECT_EQ(handler1.GetRefCount(), 0);
    EXPECT_EQ(handler2.GetRefCount(), 2);
    EXPECT_EQ(handler3.GetRefCount(), 2);
    EXPECT_EQ(PendingDeletes.Size(), 0);
    handler2.Release();
    EXPECT_EQ(handler2.GetRefCount(), 0);
    EXPECT_EQ(handler3.GetRefCount(), 1);
    EXPECT_EQ(PendingDeletes.Size(), 0);
    handler3.Release();
    EXPECT_EQ(handler3.GetRefCount(), 0);
    EXPECT_EQ(PendingDeletes.Size(), 1);
    auto deletedResource = PendingDeletes.Consume();
    EXPECT_NE(deletedResource, nullptr);
    EXPECT_EQ(PendingDeletes.Size(), 0);
}