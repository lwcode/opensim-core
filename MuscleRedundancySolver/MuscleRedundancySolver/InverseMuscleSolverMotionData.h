#ifndef TOMU_MOTIONDATA_H
#define TOMU_MOTIONDATA_H

#include <OpenSim/Common/GCVSplineSet.h>
#include <OpenSim/Common/TimeSeriesTable.h>

#include <Eigen/Dense>

namespace OpenSim {

class Model;

/// This class contains the motion data required to solve inverse problems
/// for muscle activity:
///   - inverse dynamics net joint moments
///   - muscle-tendon lengths
///   - moment arms
/// This class also allows interpolating these data for use by solvers.
class InverseMuscleSolverMotionData {
public:
    InverseMuscleSolverMotionData() = default;
    /// From the given kinematics trajectory (joint angles), this constructor
    /// will perform inverse dynamics and a muscle analysis to provide net
    /// joint moments, muscle-tendon lengths, and moment arms.
    /// The inverse dynamics moments are filtered with the provided lowpass
    /// cutoff frequency; use -1 to not filter.
    InverseMuscleSolverMotionData(const Model& model,
            const TimeSeriesTable& kinematicsData,
            const double& lowpassCutoffJointMoments,
            const double& initialTime, const double& finalTime);
    /// From the given kinematics trajectory (joint angles), this constructor
    /// will perform a muscle analysis to provide net
    /// joint moments, muscle-tendon lengths, and moment arms.
    /// The inverse dynamics net generalized forces are taken from the provided
    /// table rather than computed from the provided kinematics.
    InverseMuscleSolverMotionData(const OpenSim::Model& model,
            const TimeSeriesTable& kinematicsData,
            const TimeSeriesTable& netGeneralizedForcesData,
            const double& initialTime, const double& finalTime);
    /// Get the initial time to use in the solver.
    double getInitialTime() const { return _initialTime; }
    /// Get the final time to use in the solver.
    double getFinalTime() const { return _finalTime; }
    /// Interpolate the desired net joint moments at the provided times.
    /// @param times The times at which to interpolate; must be between the
    ///     initial and final times.
    /// @param[in,out] desiredNetGenForces Inverse dynamics net joint moments
    ///     Dimensions: degrees of freedom x time.
    void interpolateNetGeneralizedForces(const Eigen::VectorXd& times,
            Eigen::MatrixXd& desiredNetGenForces) const;
    /// Interpolate the muscle-tendon lengths at the provided times.
    /// @param times The times at which to interpolate; must be between the
    ///     initial and final times.
    /// @param[in,out] muscleTendonLengths Expressed in meters.
    ///     Dimensions: muscles x time.
    void interpolateMuscleTendonLengths(const Eigen::VectorXd& times,
            Eigen::MatrixXd& muscleTendonLengths) const;
    /// Interpolate the muscle-tendon velocities at the provided times.
    /// @param times The times at which to interpolate; must be between the
    ///     initial and final times.
    /// @param[in,out] muscleTendonVelocities Expressed in meters/second.
    ///     Dimensions: muscles x time.
    void interpolateMuscleTendonVelocities(const Eigen::VectorXd& times,
            Eigen::MatrixXd& muscleTendonVelocities) const;
    /// Interpolate the moment arms at the provided times.
    /// @param times The times at which to interpolate; must be between the
    ///     initial and final times.
    /// @param[in,out] momentArms Elements of the std::vector correspond to
    ///     times, and each element of the std::vectorhas dimensions
    ///     (degrees of freedom) x muscles
    void interpolateMomentArms(const Eigen::VectorXd& times,
            std::vector<Eigen::MatrixXd>& momentArms) const;
private:
    // This constructor performs the muscle analysis.
    InverseMuscleSolverMotionData(const Model& model,
            const TimeSeriesTable& kinematicsData,
            const double& initialTime, const double& finalTime);
    void computeInverseDynamics(const OpenSim::Model& model,
            const TimeSeriesTable& kinematicsData,
            const double& lowpassCutoffJointMoments);
    double _initialTime;
    double _finalTime;
    size_t _numDOFs;
    size_t _numActiveMuscles;
    GCVSplineSet _netGeneralizedForces;
    GCVSplineSet _muscleTendonLengths;
    GCVSplineSet _muscleTendonVelocities;
    // The vector contains an entry for each coordinate; the spline set is
    // across muscles.
    std::vector<GCVSplineSet> _momentArms;
};

} // namespace OpenSim

#endif // TOMU_MOTIONDATA_H