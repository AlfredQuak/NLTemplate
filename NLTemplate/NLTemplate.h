#ifndef __Template_H__
#define __Template_H__


#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <memory>


namespace NL {

namespace Template {

namespace Private {


struct Token {
    int type;
    std::string value;
};


class Tokenizer {
public:
    Tokenizer( const std::shared_ptr<char> & text ); // Tokenizer will free() the text on exit
    Token next();

private:
    std::shared_ptr<char> text_ptr;
    const char *text;
    long len;
    long pos;
    Token peek;
    bool peeking;
};


class Dictionary {
public:
    const std::string find( const std::string & name ) const;
    void set( const std::string & name, const std::string & value );

protected:
    std::vector<std::pair<std::string, std::string> > properties;
};


class Fragment {
public:
    virtual void render( std::ostream & output, const Dictionary & dictionary ) const = 0;
    virtual ~Fragment();
    virtual Fragment *copy() const = 0;
    virtual bool isBlockNamed( const std::string & name ) const;
};


class Text : public Fragment {
public:
    Text( const std::string & text );
    void render( std::ostream & output, const Dictionary & dictionary ) const;
    Fragment *copy() const;

private:
    const std::string text;
};


class Property : public Fragment {
public:
    Property( const std::string & name );
    void render( std::ostream & output, const Dictionary & dictionary ) const;
    Fragment *copy() const;

private:
    const std::string name;
};


} // namespace Private



class Block;

//using namespace Private;


class Node : public Private::Fragment, public Private::Dictionary {
public:
    ~Node();
    Fragment *copy() const;
    void render( std::ostream & output, const Private::Dictionary & dictionary ) const;
    Block & block( const std::string & name ) const;

protected:
    std::vector<Fragment*> fragments;

    friend class Template;
};


class Block : public Node {
public:
    Block( const std::string & name );
    Fragment *copy() const;
    ~Block();
    bool isBlockNamed( const std::string & name ) const;
    void enable();
    void disable();
    void repeat( size_t n );
    Node & operator[]( size_t index );
    void render( std::ostream & output, const Dictionary & dictionary ) const;

protected:
    const std::string name;
    bool enabled;
    bool resized;
    std::vector<Node*> nodes;
};


class Loader {
public:
    virtual ~Loader();
    virtual std::shared_ptr<char> load( const std::string & name ) = 0;
};


class LoaderFile : public Loader {
public:
    std::shared_ptr<char> load( const std::string & name );
};



class Template : public Block {
public:
    Template( Loader & loader );
    void clear();
    void load( const std::string & name );
    void render( std::ostream & output ) const;

private:
    Loader & loader;

    void load_recursive( const std::string & name, std::vector<Private::Tokenizer> & files, std::vector<Node*> & nodes );
};



} // namespace Template

} // namespace NL

#endif
