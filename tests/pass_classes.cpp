#include <catch2/catch.hpp>
#include <wrenbind17/wrenbind17.hpp>

namespace wren = wrenbind17;

class Vector3f;
std::vector<Vector3f*> instancesV3f;

class Vector3f {
public:
    Vector3f() {
        instancesV3f.push_back(this);
    }
    Vector3f(double x, double y, double z) : x(x), y(y), z(z) {
        instancesV3f.push_back(this);
    }
    Vector3f(const Vector3f& other) {
        instancesV3f.push_back(this);
        *this = other;
    }
    ~Vector3f() = default;

    void set(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    double length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    void setFromCopy(Vector3f other) {
        *this = other;
    }

    void setFromConstCopy(const Vector3f other) {
        *this = other;
    }

    void setFromRef(Vector3f& other) {
        *this = other;
    }

    void setFromConstRef(const Vector3f& other) {
        *this = other;
    }

    void setFromPtr(Vector3f* other) {
        *this = *other;
    }

    void setFromConstPtr(const Vector3f* other) {
        *this = *other;
    }

    Vector3f& operator=(const Vector3f& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    double x = 0.0f;
    double y = 0.0f;
    double z = 0.0f;
};

#define TEST_SETUP_VECTOR                                                                                              \
    wren::VM vm;                                                                                                       \
    auto& m = vm.module("test");                                                                                       \
    auto& cls = m.klass<Vector3f>("Vector3f");                                                                           \
    cls.ctor<double, double, double>();

TEST_CASE("Pass class from Wren to C++") {
    const std::string code = R"(
        import "test" for Vector3f
        var A = Vector3f.new(1.1, 2.2, 3.3)
        class Foo {
            static baz() {
                A.set(A)
            }
        }
    )";

    SECTION("Copy") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromCopy>("set");
        vm.runFromSource("main", code);
        auto baz = vm.find("main", "Foo").func("baz()");
        baz();
        REQUIRE(instancesV3f.size() >= 2);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();

    SECTION("Const copy") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromConstCopy>("set");
        vm.runFromSource("main", code);
        auto baz = vm.find("main", "Foo").func("baz()");
        baz();
        REQUIRE(instancesV3f.size() >= 2);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();

    SECTION("Reference") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromRef>("set");
        vm.runFromSource("main", code);
        auto baz = vm.find("main", "Foo").func("baz()");
        baz();
        REQUIRE(instancesV3f.size() == 1);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();

    SECTION("Const reference") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromConstRef>("set");
        vm.runFromSource("main", code);
        auto baz = vm.find("main", "Foo").func("baz()");
        baz();
        REQUIRE(instancesV3f.size() == 1);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();

    SECTION("Pointer") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromPtr>("set");
        vm.runFromSource("main", code);
        auto baz = vm.find("main", "Foo").func("baz()");
        baz();
        REQUIRE(instancesV3f.size() == 1);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();

    SECTION("Const pointer") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromConstPtr>("set");
        vm.runFromSource("main", code);
        auto baz = vm.find("main", "Foo").func("baz()");
        baz();
        REQUIRE(instancesV3f.size() == 1);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();
}

TEST_CASE("Pass class from C++ to Wren") {
    const std::string code = R"(
        import "test" for Vector3f
        var A = Vector3f.new(0.0, 0.0, 0.0)
        class Foo {
            static baz(other) {
                A.set(other)
            }
        }
    )";

    SECTION("Copy") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromConstRef>("set");

        vm.runFromSource("main", code);
        Vector3f value(1.1, 2.2, 3.3);
        auto baz = vm.find("main", "Foo").func("baz(_)");
        baz(value);

        // REQUIRE(instancesV3f.size() == 4);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();

    SECTION("Const copy") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromConstRef>("set");

        vm.runFromSource("main", code);
        const Vector3f value(1.1, 2.2, 3.3);
        auto baz = vm.find("main", "Foo").func("baz(_)");
        baz(value);

        // REQUIRE(instancesV3f.size() == 4);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();

    SECTION("Reference") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromConstRef>("set");

        vm.runFromSource("main", code);
        Vector3f value(1.1, 2.2, 3.3);
        auto& ref = value;
        auto baz = vm.find("main", "Foo").func("baz(_)");
        baz(ref);

        // REQUIRE(instancesV3f.size() == 4);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();

    SECTION("Const reference") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromConstRef>("set");

        vm.runFromSource("main", code);
        Vector3f value(1.1, 2.2, 3.3);
        const auto& ref = value;
        auto baz = vm.find("main", "Foo").func("baz(_)");
        baz(ref);

        // REQUIRE(instancesV3f.size() == 4);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();

    SECTION("Pointer") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromConstRef>("set");

        vm.runFromSource("main", code);
        Vector3f value(1.1, 2.2, 3.3);
        auto baz = vm.find("main", "Foo").func("baz(_)");
        baz(&value);

        // REQUIRE(instancesV3f.size() == 4);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();

    SECTION("Const pointer") {
        TEST_SETUP_VECTOR
        cls.func<&Vector3f::setFromConstRef>("set");

        vm.runFromSource("main", code);
        Vector3f value(1.1, 2.2, 3.3);
        const auto* ptr = &value;
        auto baz = vm.find("main", "Foo").func("baz(_)");
        baz(ptr);

        // REQUIRE(instancesV3f.size() == 4);
        REQUIRE(instancesV3f[0]->x == Approx(1.1));
        REQUIRE(instancesV3f[0]->y == Approx(2.2));
        REQUIRE(instancesV3f[0]->z == Approx(3.3));
    }
    instancesV3f.clear();
}

class Widget;
std::vector<Widget*> widgets;

class Widget {
public:
    Widget() = delete;
    Widget(std::string name) : name(std::move(name)) {
        widgets.push_back(this);
    }
    Widget(Widget&& other) = delete;
    Widget(const Widget& other) = delete;
    ~Widget() = default;

    void set(const Widget& other) {
        name = other.name;
    }

    Widget& operator=(const Widget& other) = delete;
    Widget& operator=(Widget&& other) = delete;

    std::string name;
};

TEST_CASE("Pass class from C++ to Wren as shared_ptr") {
    const std::string code = R"(
        import "test" for Widget
        var A = Widget.new("Ahoj")
        class Foo {
            static baz(other) {
                A.set(other)
            }
        }
    )";

    SECTION("std::shared_ptr") {
        wren::VM vm;
        auto& m = vm.module("test");
        auto& cls = m.klass<Widget>("Widget");
        cls.ctor<std::string>();
        cls.func<&Widget::set>("set");

        vm.runFromSource("main", code);
        auto ptr = std::make_shared<Widget>("Hello World");
        auto baz = vm.find("main", "Foo").func("baz(_)");
        baz(ptr);

        REQUIRE(widgets.size() == 2);
        REQUIRE(widgets[0]->name == "Hello World");
    }
    instancesV3f.clear();
}
