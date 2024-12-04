implementing avio_core;

public namespace avio {
  public func add_two_numbers<T>(a: T, b: T) -> T where T: IArithmetic {
    return a + b;
  }
}