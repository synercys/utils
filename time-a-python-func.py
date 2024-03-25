import time

def timing_decorator(func):
    def wrapper(*args, **kwargs):
        start = time.time()
        original_return_val = func(*args, **kwargs)
        end = time.time()
        print("time elapsed in ", func.__name__, ": ", end - start, "in seconds", sep='')
        return original_return_val

    return wrapper


@timing_decorator
def function_to_time():
    time.sleep(1)


function_to_time()
