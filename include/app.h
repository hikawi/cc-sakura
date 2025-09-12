/**
 * \file app.h
 *
 * Represents the final application state as a state machine.
 */

#pragma once

namespace ccsakura
{

/**
 * Virtual interface for \ref ccsakura::app for mocking purposes.
 */
class iapp
{
  public:
    virtual ~iapp() = default;
};

/**
 * Represents the application with a state.
 */
class app : public iapp
{
  public:
    app();
    ~app() override;
};

}; // namespace ccsakura
