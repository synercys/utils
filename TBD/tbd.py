import numpy as np
from sklearn.mixture import GaussianMixture
from scipy.stats import multivariate_normal

def get_d(p1:type[GaussianMixture], p2:type[GaussianMixture], n_components:int):
    output = 0
    new_weights = p1.weights_ * p2.weights_
    new_means = p1.means_ - p2.means_
    new_covars = p1.covariances_ + p2.covariances_
    for i in range(n_components):
        output += multivariate_normal.pdf(np.zeros(2), mean=new_means[i], cov=new_covars[i])
    return output

def get_tbd(p_r, p_q):
    n_components = p_r.n_components
    d_r = get_d(p_r, p_r, n_components)
    d_q = get_d(p_q, p_q, n_components)
    d_rq = 2*get_d(p_r, p_q, n_components)
    return (d_r + d_q - d_rq)/np.sqrt(1+4*d_rq)

if __name__ == '__main__':
    # Example data
    # Reference shape: 200 points of outline in 2D space 
    R = np.random.rand(200,2)
    # Query shape: 123 points of outline in 2D space 
    Q = np.random.rand(123,2)

    # Profile reference GMM
    # Select appropriate number of gaussians to represent the shape
    n_components = 10
    p_r = GaussianMixture(n_components)
    p_r.fit(R)

    # Profile query GMM
    p_q = GaussianMixture(n_components)
    p_q.fit(Q)

    # Get TBD distance between the two shapes
    print(get_tbd(p_r, p_q))

