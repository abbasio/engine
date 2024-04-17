-- Lua global variable
test_variable = 5 * 2
user_name = "test mctesterson"

print ("Hello "..user_name)
if test_variable == 10 then
    print("The value of the test variable is "..test_variable)
end

-- Lua table
config = {
    title = "engine",
    fullscreen = false,
    resolution = {
        width = 800,
        height = 600
    }
}

-- Lua function

function factorial(n)
    if n == 0 then
        return 1
    else
        return n * factorial(n - 1)
    end
end

