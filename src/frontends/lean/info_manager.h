/*
Copyright (c) 2014 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#pragma once
#include <memory>
#include <vector>
#include "util/thread.h"
#include "kernel/expr.h"
#include "kernel/metavar.h"
#include "library/io_state_stream.h"

namespace lean {
class info_data {
    unsigned m_line;
    unsigned m_column;
public:
    info_data():m_line(0), m_column(0) {}
    info_data(unsigned l, unsigned c):m_line(l), m_column(c) {}
    virtual ~info_data() {}
    unsigned get_line() const { return m_line; }
    unsigned get_column() const { return m_column; }
    bool eq_pos(unsigned line, unsigned col) const { return m_line == line && m_column == col; }
    virtual void display(io_state_stream const & ios) const = 0;
    virtual void instantiate(substitution &) {}
};
bool operator<(info_data const & i1, info_data const & i2);

class type_info_data : public info_data {
protected:
    expr m_expr;
public:
    type_info_data() {}
    type_info_data(unsigned l, unsigned c, expr const & e):info_data(l, c), m_expr(e) {}
    expr const & get_type() const { return m_expr; }
    virtual void display(io_state_stream const & ios) const;
    virtual void instantiate(substitution & s) { m_expr = s.instantiate(m_expr); }
};

class synth_info_data : public type_info_data {
public:
    synth_info_data(unsigned l, unsigned c, expr const & e):type_info_data(l, c, e) {}
    virtual void display(io_state_stream const & ios) const;
};

class overload_info_data : public info_data {
    expr m_choices;
public:
    overload_info_data(unsigned l, unsigned c, expr const & e):info_data(l, c), m_choices(e) {}
    virtual void display(io_state_stream const & ios) const;
};

class coercion_info_data : public info_data {
    expr m_coercion;
public:
    coercion_info_data(unsigned l, unsigned c, expr const & e):info_data(l, c), m_coercion(e) {}
    virtual void display(io_state_stream const & ios) const;
};

class info_manager {
    typedef std::vector<std::unique_ptr<info_data>> data_vector;
    mutex       m_mutex;
    unsigned    m_sorted_upto;
    data_vector m_data;
    void add_core(std::unique_ptr<info_data> && d);
    unsigned find(unsigned line, unsigned column);
    void sort_core();
public:
    info_manager();
    void invalidate(unsigned sline);
    void add(std::unique_ptr<info_data> && d);
    void append(std::vector<std::unique_ptr<info_data>> && v, bool remove_duplicates = true);
    void append(std::vector<type_info_data> & v, bool remove_duplicates);
    void sort();
    void display(io_state_stream const & ios, unsigned line);
};
}
