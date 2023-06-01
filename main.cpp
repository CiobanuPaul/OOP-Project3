//I chose to use a factory design pattern in order to adopt different type of animals.
//I chose to use a decorator design pattern class named GrowUp because animals can grow up and develop their actual functions.
//The factory class also has methods related to the decorator, managing which animals grow up.


#include <iostream>
#include <memory>

class AnimalTooOld : public std::exception{
public:
    AnimalTooOld() = default;
    const char * what() const noexcept override{
        return "The animal is too old and cannot get older!";
    }
};

class MonkeyTooGreedy : public std::exception{
public:
    MonkeyTooGreedy() = default;
    const char * what() const noexcept override{
        return "Greedy monkey wants too much food!";
    }
};



class IDGen{
protected:
    static int nr;
    const int id;
public:
    IDGen() : id(++nr){}
};

int IDGen::nr = 0;




class Animal : public IDGen{
protected:
    std::string name;
    int age;
public:
    virtual void talk() = 0;
    virtual ~Animal(){};
    Animal(const int &age, const std::string &name) : age(age), name(name){}

    int getAge() const{
        return age;
    }

    std::string getName() const{
        return name;
    }

    virtual void print() const{
        std::cout<<"Name: "<<name<<"\n";
        std::cout<<"Age: "<<age<<"\n";
    }
};




class Lion : public Animal{
public:
    void talk() override{
        std::cout<<"Rooar!";
    }
    Lion(const int &age, const std::string &name) : Animal(age, name){}
    ~Lion() override {};
};




class Monkey : public virtual Animal{
protected:
    std::string toy;
    int greedCounter;
public:
    void talk() override{
        greedCounter = 0;
        std::cout<<"U a aa!";
    }

    virtual void askFood(){
        greedCounter++;
        if(greedCounter > 4)
            throw MonkeyTooGreedy();
        std::cout<<"Foood!";
    }

    Monkey(const int &age, const std::string &name, const std::string &toy) : Animal(age, name), toy(toy){
        greedCounter = 0;
    }
    virtual ~Monkey() override {};
    
    std::string getToy() const{
        return toy;
    }

    void print() const override{
        Animal::print();
        std::cout<<"Toy: "<<toy<<"\n";
    }
};




class Snake : public Animal{
public:
    void talk() override{
        std::cout<<"Hiss!";
    }

    Snake(const int &age, const std::string &name) : Animal(age, name){}
    ~Snake() override {};
};



class GrownUp : public virtual Animal{
protected:
    std::shared_ptr<Animal> a;
    int growth;

public:

    GrownUp(const std::shared_ptr<Animal>& a) : Animal(a->getAge(), a->getName()), a(a){
        auto p = std::dynamic_pointer_cast<GrownUp>(a);
        if(p == nullptr)
            growth = 1;
        else {
            if(p->getGrowth() == 3)
                throw AnimalTooOld();
            growth = p->getGrowth() + 1;
        }
        this->age++;
    }

    virtual ~GrownUp() override {}

    void talk() override{
        a->talk();
        std::cout<<" ";
        a->talk();
    }

    int getGrowth() const {
        return growth;
    };

};




class GrownMonkey : public GrownUp, public Monkey{
public:                             //Monkey
    GrownMonkey(const std::shared_ptr<Animal>& m) : GrownUp(m), Monkey(m->getAge(), m->getName(),
                                                                       std::dynamic_pointer_cast<Monkey>(m)->getToy()),
                                                    Animal(m->getAge(), m->getName()){


        this->age++;
    }
    ~GrownMonkey() override {};

    void talk() override{
        GrownUp :: talk();
    }

    void askFood() override{
        auto m = std::dynamic_pointer_cast<Monkey>(a);
        m->askFood();
        std::cout<<" ";
        try {
            m->askFood();
        }
        catch (MonkeyTooGreedy&){
            std::cout<<"I learned to be moderate.";
        }
    }
};




template<class A>
class AdminAnimal{
    AdminAnimal() = default;
    AdminAnimal(const AdminAnimal& obj) = delete;
    static std::unique_ptr<AdminAnimal<A>> uniqueinstance;

public:

    std::shared_ptr<A> adoptAnimal(const int& age, const std::string& name){
        return std::make_shared<A>(age, name);
    }

    std::shared_ptr<GrownUp> growUp(std::shared_ptr<A> a){
        return std::make_shared<GrownUp>(a);
    }

    std::shared_ptr<Monkey> adoptAnimal(const int &age, const std::string &name, const std::string &toy);

    void print() const{}

    static std::unique_ptr<AdminAnimal<A>>& getInstance(){
        if(uniqueinstance == nullptr)
            uniqueinstance = std::unique_ptr<AdminAnimal<A>>(new AdminAnimal<A>);
        return uniqueinstance;
    }
    
};
template <class A>
std::unique_ptr<AdminAnimal<A>> AdminAnimal<A>::uniqueinstance = nullptr;

template<>
std::shared_ptr<Monkey> AdminAnimal<Monkey>::adoptAnimal(const int &age, const std::string &name, const std::string& toy) {
    return std::make_shared<Monkey>(age, name, toy);
}

template<>
std::shared_ptr<GrownUp> AdminAnimal<Monkey>::growUp(std::shared_ptr<Monkey> m){
    return std::static_pointer_cast<GrownUp>(std::make_shared<GrownMonkey>(m));
}



template <class A>
A& clone(const A obj){
    return obj;
}

std::ostream& operator<<(std::ostream& os, Animal& obj){
    obj.print();
}

template <class A>
std::ostream& operator<<(std::ostream& os, AdminAnimal<A>& obj){
    obj.print();
}




int main() {
    auto& lions = AdminAnimal<Lion>::getInstance();
    auto& monkeys = AdminAnimal<Monkey>::getInstance();
    auto& snakes = AdminAnimal<Snake>::getInstance();

    auto l = lions->adoptAnimal(12, "Leo");
    std::cout<<"Lion:\n";
    std::cout<<*l;
    l->talk(); std::cout<<"\n\n";

    auto s = snakes->adoptAnimal(3, "Cleope");
    std::cout<<"Snake:\n";
    std::cout<<*s;
    l->talk(); std::cout<<"\n\n";

    auto m = monkeys->adoptAnimal(12, "Bongo", "stick");
    std::cout<<"Monkey:\n";
    std::cout<<*m;
    m->talk(); std::cout<<"\n";
    m->askFood(); std::cout<<"\n\n";


    auto gl = lions->growUp(l);
    std::cout<<"GrownLion "<<gl->getName()<<":\n";
    gl->talk(); std::cout<<"\n\n";


    auto gm = monkeys->growUp(m);
    std::cout<<"GrownMonkey "<<gm->getName()<<":\n";
    gm->talk(); std::cout<<"\n";
    auto gupm = std::dynamic_pointer_cast<GrownMonkey>(gm);
    gupm->askFood(); std::cout<<"\n\n";

    auto ggm = monkeys->growUp(std::static_pointer_cast<Monkey>(gupm));
    std::cout<<"GrownGrownMonkey "<<ggm->getName()<<":\n";
    ggm->talk(); std::cout<<"\n";
    auto gupgupm = std::dynamic_pointer_cast<GrownMonkey>(ggm);
    gupgupm->askFood(); std::cout<<"\n";
    try {
        m->askFood();  //ask for food just one more time,  monkey m <=> monkey gupgupm
    }
    catch(MonkeyTooGreedy &err){
        std::cout<<err.what();
    }
    std::cout<<"\n\n";

    auto g3m = monkeys->growUp(std::static_pointer_cast<Monkey>(gupgupm));
    std::cout<<"GrownGrownGrownMonkey "<<g3m->getName()<<":\n";
    g3m->talk(); std::cout<<"\n";
    auto gup3m = std::dynamic_pointer_cast<GrownMonkey>(g3m);
    gup3m->askFood(); std::cout<<"\n\n";

    std::cout<<"Trying to grow up the monkey even more...\n";
    try{
        auto g4m = monkeys->growUp(std::static_pointer_cast<Monkey>(gup3m));
    }
    catch(AnimalTooOld& err){
        std::cout<<err.what();
    }
    return 0;
}
