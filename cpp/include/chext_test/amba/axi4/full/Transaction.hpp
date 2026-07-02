#ifndef CHEXT_TEST_AMBA_AXI4_FULL_TRANSACTION_HPP_INCLUDED
#define CHEXT_TEST_AMBA_AXI4_FULL_TRANSACTION_HPP_INCLUDED

#include <chext_test/amba/axi4/full/Driver.hpp>
#include <chext_test/util/Spawn.hpp>
#include <chext_test/util/Util.hpp>

#include <systemc>

#include <memory>

namespace chext_test::amba::axi4::full {

struct Beat {
    /// @brief index of the beat within the transaction
    uint32_t index;

    /// @brief the memory address of the first meaningful byte in the transaction
    uint64_t addr;

    /// @brief if the beat is last
    bool last;

    /// @brief lower byte index of the meaningful bus data
    uint32_t lowerByteIndex;

    /// @brief upper byte index of the meaningful bus data
    uint32_t upperByteIndex;

    /// @brief number of meaningful bytes in the beat
    uint32_t size;

    /// @brief write strobe corresponding to the beat
    uint64_t strb;

    JQR_DECL(
        Beat,
        JQR_MEMBER(index),
        JQR_MEMBER(addr, jqr::opts::dump_fmt { "{:#018x}" }),
        JQR_MEMBER(last, jqr::opts::dump_fmt { "{:#03b}" }),
        JQR_MEMBER(lowerByteIndex),
        JQR_MEMBER(upperByteIndex),
        JQR_MEMBER(size, jqr::opts::dump_fmt { "{:#04x}" }),
        JQR_MEMBER(strb, jqr::opts::dump_fmt { "{:#066b}" })
    )

    JQR_TO_STRING
};

struct Transaction {
    Transaction(uint16_t wData, bool axi3Compat = false)
        : wData_ { wData }
        , axi3Compat_ { axi3Compat } {
        assert(wData <= 512);
    }

    void reset(uint64_t addr, uint8_t len, uint8_t size, uint8_t burst) {
        assert(burst == 0 || burst == 1 || burst == 2);

        if (axi3Compat_) {
            assert(len < 16 && "in axi3 mode, len < 16!");
        }

        if (burst == 2 /* WRAP */) {
            assert(len == 0 || len == 1 || len == 3 || len == 7 || len == 15);
            uint8_t log2numBeats = log2(len + 1);

            // the total size of the transfer should be aligned
            uint64_t transferSize = ((uint64_t)1) << size;
            assert((addr & (transferSize - 1)) == 0 && "for wrap burst, it must be aligned to the transfer size!");
        }

        firstAddr_ = addr;
        addr_ = addr >> size;
        len_ = len;
        size_ = size;
        burst_ = burst;

        beatIndex_ = 0;
    }

    bool nextBeat(Beat& beat) {
        if (beatIndex_ > len_)
            return false;

        beat.~Beat();

        uint64_t busMask = (wData_ / 8) - 1;

        if (beatIndex_ == 0 || burst_ == 0) {
            uint64_t addr = firstAddr_;

            uint32_t lowerByteIndex = addr & busMask;
            uint32_t upperByteIndex = (((1 + (addr >> size_)) << size_) - 1) & busMask;
            uint32_t size = (upperByteIndex - lowerByteIndex) + 1;
            uint64_t strb = ((((uint64_t)1) << size) - 1) << lowerByteIndex;

            new (&beat) Beat {
                .index = beatIndex_,
                .addr = addr,
                .last = (beatIndex_ == len_),
                .lowerByteIndex = lowerByteIndex,
                .upperByteIndex = upperByteIndex,
                .size = size,
                .strb = strb
            };
        } else {
            uint64_t addr = addr_ << size_;

            uint32_t lowerByteIndex = addr & busMask;
            uint32_t upperByteIndex = (((1 + (addr >> size_)) << size_) - 1) & busMask;
            uint32_t size = (upperByteIndex - lowerByteIndex) + 1;
            uint64_t strb = ((((uint64_t)1) << size) - 1) << lowerByteIndex;

            new (&beat) Beat {
                .index = beatIndex_,
                .addr = addr,
                .last = (beatIndex_ == len_),
                .lowerByteIndex = lowerByteIndex,
                .upperByteIndex = upperByteIndex,
                .size = size,
                .strb = strb
            };
        }

        if (burst_ == 0 /* FIXED */) {
            // do nothing
        } else if (burst_ == 1 /* INCR */) {
            addr_++;
        } else /* WRAP */ {
            uint64_t mask = len_;
            addr_ = ((addr_ + 1) & mask) | ((addr_) & ~mask);
        }

        beatIndex_++;

        return true;
    }

private:
    uint16_t wData_;
    bool axi3Compat_;

    unsigned beatIndex_;

    uint64_t firstAddr_;
    uint64_t addr_;

    uint8_t len_;
    uint8_t size_;
    uint8_t burst_;
};

namespace detail {

inline void prepareWriteData(sc_dt::sc_bv_base& out, uint8_t const* in, uint8_t lowerByteIndex, uint8_t upperByteIndex) {
    // can be further improved
    uint8_t buffer[128] = { 0 };

    for (unsigned i = lowerByteIndex; i <= upperByteIndex; ++i)
        buffer[i] = *(in++);

    static_assert(sizeof(sc_dt::sc_digit) == 4);

    for (unsigned wordIndex = 0; wordIndex < out.size(); ++wordIndex) {
        // clang-format off
        out.set_word(
            wordIndex,
            (((sc_dt::sc_digit)buffer[wordIndex * 4]) << 0) |
            (((sc_dt::sc_digit)buffer[wordIndex * 4 + 1]) << 8) |
            (((sc_dt::sc_digit)buffer[wordIndex * 4 + 2]) << 16) |
            (((sc_dt::sc_digit)buffer[wordIndex * 4 + 3]) << 24)
        );
        // clang-format on
    }
}

inline void prepareWriteStrobe(sc_dt::sc_bv_base& out, uint8_t lowerByteIndex, uint8_t upperByteIndex) {
    for (unsigned bitIndex = 0; bitIndex < out.length(); ++bitIndex) {
        out.set_bit(bitIndex, bitIndex >= lowerByteIndex && bitIndex <= upperByteIndex);
    }
}

inline void prepareReadData(sc_dt::sc_bv_base const& in, uint8_t* out, uint8_t lowerByteIndex, uint8_t upperByteIndex) {
    // can be further improved
    uint8_t buffer[128] = { 0 };

    static_assert(sizeof(sc_dt::sc_digit) == 4);

    for (unsigned wordIndex = 0; wordIndex < in.size(); ++wordIndex) {
        auto word = in.get_word(wordIndex);

        // clang-format off
        buffer[wordIndex * 4] = (word >> 0) & 0xFF;
        buffer[wordIndex * 4 + 1] = (word >> 8) & 0xFF;
        buffer[wordIndex * 4 + 2] = (word >> 16) & 0xFF;
        buffer[wordIndex * 4 + 3] = (word >> 24) & 0xFF;
        // clang-format on
    }

    for (unsigned i = lowerByteIndex; i <= upperByteIndex; ++i)
        *(out++) = buffer[i];
}

} // namespace detail

inline void simpleWrite(
    SlaveBase& target,
    uint64_t& addr,
    uint64_t& numBytes,
    uint8_t const*& data,
    int size = -1,
    bool log = false
) {
    auto const& cfg = target.config;
    unsigned maxSize = log2(cfg.wData / 8u);
    size = (size >= 0 && size <= maxSize) ? size : maxSize;

    uint64_t mask = (((uint64_t)1) << size) - 1;
    uint64_t alignedAddr = addr & ~mask;
    uint64_t alignedNumBytes = numBytes + addr - alignedAddr;

    uint64_t numBeats = alignedNumBytes >> size;
    if (alignedNumBytes & mask)
        numBeats++;

    uint8_t len = numBeats - 1;
    len = MIN(len, cfg.axi3Compat ? 15 : 255);

    Transaction transaction(cfg.wData, cfg.axi3Compat);
    transaction.reset(addr, len, size, 1);

    // TODO: do this without spawning new threads all the time!
    sc_core::sc_join j;

    SC_SPAWN_TO(j) {
        Packets::WriteAddress aw { cfg };
        aw.id = util::bv_from(0, cfg.wId);
        aw.addr = util::bv_from(addr, cfg.wAddr);
        aw.len = (uint8_t)len;
        aw.size = (uint8_t)size;
        aw.burst = 1;

        target.sendAW(aw);
        if (log)
            fmt::print("simpleWrite: [t = {}] sent: {}\n", sc_core::sc_time_stamp().to_string(), aw);
    };

    SC_SPAWN_TO(j) {
        sc_dt::sc_bv_base bvData((int)cfg.wData);
        sc_dt::sc_bv_base bvStrb((int)cfg.wStrobe);

        Beat b;

        while (transaction.nextBeat(b)) {
            uint32_t transferSize = MIN(b.size, numBytes);
            detail::prepareWriteData(bvData, data, b.lowerByteIndex, b.lowerByteIndex + transferSize - 1);
            detail::prepareWriteStrobe(bvStrb, b.lowerByteIndex, b.lowerByteIndex + transferSize - 1);

            Packets::WriteData w { cfg };
            w.data = bvData;
            w.strb = bvStrb;
            w.last = b.last;

            target.sendW(w);
            if (log)
                fmt::print("simpleWrite: [t = {}] sent: {}\n", sc_core::sc_time_stamp().to_string(), w);

            addr += transferSize;
            data += transferSize;
            numBytes -= transferSize;
        }
    };

    SC_SPAWN_TO(j) {
        auto b = target.receiveB();

        if (log)
            fmt::print("simpleWrite: [t = {}] received: {}\n", sc_core::sc_time_stamp().to_string(), b);
    };

    j.wait();
}

inline void write(
    SlaveBase& target,
    uint64_t addr,
    uint64_t numBytes,
    unsigned char const* data,
    int size = -1,
    bool log = false
) {
    while (numBytes > 0) {
        simpleWrite(target, addr, numBytes, data, size, log);
        sc_core::wait(sc_core::SC_ZERO_TIME);
    }
}

inline void simpleRead(
    SlaveBase& target,
    uint64_t& addr,
    uint64_t& numBytes,
    uint8_t*& data,
    int size = -1,
    bool log = false
) {
    auto const& cfg = target.config;
    unsigned maxSize = log2(cfg.wData / 8u);
    size = (size >= 0 && size <= maxSize) ? size : maxSize;

    uint64_t mask = (((uint64_t)1) << size) - 1;
    uint64_t alignedAddr = addr & ~mask;
    uint64_t alignedNumBytes = numBytes + addr - alignedAddr;

    uint64_t numBeats = alignedNumBytes >> size;
    if (alignedNumBytes & mask)
        numBeats++;

    uint8_t len = numBeats - 1;

    len = MIN(len, cfg.axi3Compat ? 15 : 255);

    Transaction transaction(cfg.wData, cfg.axi3Compat);
    transaction.reset(addr, len, size, 1);

    // TODO: do this without spawning new threads all the time!
    sc_core::sc_join j;

    SC_SPAWN_TO(j) {
        Packets::ReadAddress ar { cfg };
        ar.id = util::bv_from(0, cfg.wId);
        ar.addr = util::bv_from(addr, cfg.wAddr);
        ar.len = (uint8_t)len;
        ar.size = (uint8_t)size;
        ar.burst = 1;

        target.sendAR(ar);
        if (log)
            fmt::print("simpleRead: [t = {}] sent: {}\n", sc_core::sc_time_stamp().to_string(), ar);
    };

    SC_SPAWN_TO(j) {
        sc_dt::sc_bv_base bvData((int)cfg.wData);

        Beat b;

        while (transaction.nextBeat(b)) {
            auto r = target.receiveR();
            if (log)
                fmt::print("simpleRead: [t = {}] received: {}\n", sc_core::sc_time_stamp().to_string(), r);

            uint32_t transferSize = MIN(b.size, numBytes);
            detail::prepareReadData(r.data, data, b.lowerByteIndex, b.lowerByteIndex + transferSize - 1);

            addr += transferSize;
            data += transferSize;
            numBytes -= transferSize;
        }
    };

    j.wait();
}

inline void read(
    SlaveBase& target,
    uint64_t addr,
    uint64_t numBytes,
    uint8_t* data,
    int size = -1,
    bool log = false
) {
    while (numBytes > 0) {
        simpleRead(target, addr, numBytes, data, size, log);
        sc_core::wait(sc_core::SC_ZERO_TIME);
    }
}

#include <chext_test/amba/axi4/full/Packets.hpp>
#include <memory>
#include <vector>

struct MemoryHandler : std::enable_shared_from_this<MemoryHandler> {
    virtual Packets::ReadData read(
        Config const& cfg,
        Packets::ReadAddress const& ar,
        Beat const& beat
    ) = 0;

    virtual void write(
        Config const& cfg,
        Packets::WriteAddress const& aw,
        Beat const& beat,
        Packets::WriteData const& w,
        uint8_t& resp
    ) = 0;

    virtual ~MemoryHandler() = default;
};

struct BasicMemoryHandler : MemoryHandler {
    BasicMemoryHandler(uint64_t size, uint64_t base = 0)
        : data_ { new uint8_t[size] }
        , base_ { base } {
    }

    Packets::ReadData read(
        Config const& cfg,
        Packets::ReadAddress const& ar,
        Beat const& beat
    ) override {
        uint8_t buffer[128] = { 0 };
        uint64_t addr = beat.addr - base_;

        Packets::ReadData result { cfg };
        result.id = ar.id;
        result.resp = 0;
        result.last = beat.last;

        for (uint8_t i = beat.lowerByteIndex; i <= beat.upperByteIndex; ++i) {
            buffer[i] = data_[addr++];
        }

        static_assert(sizeof(sc_dt::sc_digit) == 4);

        for (unsigned wordIndex = 0; wordIndex < result.data.size(); ++wordIndex) {
            sc_dt::sc_digit word = 0;

            // clang-format off
            result.data.set_word(
                wordIndex,
                (((sc_dt::sc_digit)buffer[wordIndex * 4] << 0)) |
                (((sc_dt::sc_digit)buffer[wordIndex * 4 + 1]) << 8) |
                (((sc_dt::sc_digit)buffer[wordIndex * 4 + 2]) << 16) |
                (((sc_dt::sc_digit)buffer[wordIndex * 4 + 3]) << 24)
            );
            // clang-format on
        }

        return result;
    }

    virtual void write(
        Config const& cfg,
        Packets::WriteAddress const& aw,
        Beat const& beat,
        Packets::WriteData const& w,
        uint8_t& resp
    ) override {
        uint8_t buffer[128] = { 0 };
        uint64_t addr = beat.addr - base_;

        for (unsigned wordIndex = 0; wordIndex < w.data.size(); ++wordIndex) {
            auto word = w.data.get_word(wordIndex);

            // clang-format off
            buffer[wordIndex * 4] = (word >> 0) & 0xFF;
            buffer[wordIndex * 4 + 1] = (word >> 8) & 0xFF;
            buffer[wordIndex * 4 + 2] = (word >> 16) & 0xFF;
            buffer[wordIndex * 4 + 3] = (word >> 24) & 0xFF;
            // clang-format on
        }

        for (unsigned i = beat.lowerByteIndex; i <= beat.upperByteIndex; ++i) {
            if (w.strb.get_bit(i)) {
                data_[addr++] = buffer[i];
            } else
                addr++;
        }
    }

    uint8_t* data() const noexcept {
        return data_;
    }

    virtual ~BasicMemoryHandler() {
        delete[] data_;
    }

private:
    uint8_t* data_;
    uint64_t base_;
};

struct Memory : sc_core::sc_module {
    Memory(
        const sc_core::sc_module_name& name,
        std::shared_ptr<MemoryHandler> handler,
        uint32_t numRead,
        uint32_t numWrite,
        sc_core::sc_time latencyRead,
        sc_core::sc_time latencyWrite,
        bool waitAfterLastW = false,
        bool log = false
    )
        : sc_module { name }
        , handler_ { handler }
        , numRead_ { numRead }
        , numWrite_ { numWrite }
        , latencyRead_ { latencyRead }
        , latencyWrite_ { latencyWrite }
        , waitAfterLastW_ { waitAfterLastW }
        , log_ { log } {
        assert(handler_ != nullptr);

        arFifos_.reserve(numRead_);
        for (uint32_t i = 0; i < numRead_; ++i)
            arFifos_.emplace_back(
                new sc_core::sc_fifo<Packets::ReadAddress*>(fmt::format("arFifo_{}", i).c_str())
            );

        rFifos_.reserve(numRead_);
        for (uint32_t i = 0; i < numRead_; ++i)
            rFifos_.emplace_back(
                new sc_core::sc_fifo<Packets::ReadData*>(fmt::format("rFifo_{}", i).c_str())
            );

        awFifos_.reserve(numWrite_);
        for (uint32_t i = 0; i < numWrite_; ++i)
            awFifos_.emplace_back(
                new sc_core::sc_fifo<Packets::WriteAddress*>(fmt::format("awFifo_{}", i).c_str())
            );

        wFifos_.reserve(numWrite_);
        for (uint32_t i = 0; i < numWrite_; ++i)
            wFifos_.emplace_back(
                new sc_core::sc_fifo<Packets::WriteData*>(fmt::format("wFifo_{}", i).c_str())
            );

        bFifos_.reserve(numWrite_);
        for (uint32_t i = 0; i < numWrite_; ++i)
            bFifos_.emplace_back(
                new sc_core::sc_fifo<Packets::WriteResponse*>(fmt::format("bFifo_{}", i).c_str())
            );
    }

    void spawn(MasterBase& master) {
        sc_core::sc_spawn([&, this] { run(master); });
    }

    void run(MasterBase& master) {
        assert(!isRunning_);
        isRunning_ = true;

        sc_core::sc_join j;

        // AR process
        j.add_process(
            sc_core::sc_spawn(
                [&] {
                    uint32_t i = 0;

                    while (true) {
                        auto ar = new Packets::ReadAddress { master.receiveAR() };
                        if (log_)
                            fmt::print("Memory.ar: [t = {}] received: {}\n", sc_core::sc_time_stamp().to_string(), *ar);

                        arFifos_[i]->write(ar);

                        i = (i + 1) % numRead_;
                    }
                },
                "ar"
            )
        );

        // Actual reads
        for (uint32_t i = 0; i < numRead_; ++i) {
            j.add_process(
                sc_core::sc_spawn(
                    [&, i = i] {
                        while (true) {
                            auto ar = arFifos_[i]->read();
                            sc_core::wait(latencyRead_);

                            Transaction transaction { (uint16_t)master.config.wData, master.config.axi3Compat };
                            transaction.reset(ar->addr.to_uint64(), ar->len, ar->size, ar->burst);

                            Beat beat;
                            while (transaction.nextBeat(beat)) {
                                rFifos_[i]->write(new Packets::ReadData { handler_->read(master.config, *ar, beat) });
                            }

                            delete ar;
                        }
                    },
                    fmt::format("read_{}", i).c_str()
                )
            );
        }

        // R process
        j.add_process(
            sc_core::sc_spawn(
                [&] {
                    uint32_t i = 0;

                    while (true) {
                        bool flag = true;

                        while (flag) {
                            auto r = rFifos_[i]->read();

                            master.sendR(*r);

                            if (log_)
                                fmt::print("Memory.r: [t = {}] sent: {}\n", sc_core::sc_time_stamp().to_string(), *r);

                            flag = !r->last;
                            delete r;
                        }

                        i = (i + 1) % numRead_;
                    }
                },
                "r"
            )
        );

        // AW process
        j.add_process(
            sc_core::sc_spawn(
                [&] {
                    uint32_t i = 0;

                    while (true) {
                        auto aw = new Packets::WriteAddress { master.receiveAW() };
                        if (log_)
                            fmt::print("Memory.aw: [t = {}] received: {}\n", sc_core::sc_time_stamp().to_string(), *aw);

                        awFifos_[i]->write(aw);

                        i = (i + 1) % numWrite_;
                    }
                },
                "aw"
            )
        );

        // W process
        j.add_process(
            sc_core::sc_spawn(
                [&] {
                    uint32_t i = 0;

                    while (true) {
                        bool flag = true;

                        while (flag) {
                            auto w = new Packets::WriteData { master.receiveW() };
                            if (log_)
                                fmt::print("Memory.w: [t = {}] received: {}\n", sc_core::sc_time_stamp().to_string(), *w);

                            wFifos_[i]->write(w);

                            flag = !w->last;
                        }

                        i = (i + 1) % numWrite_;
                    }
                },
                "w"
            )
        );

        // Actual writes
        for (uint32_t i = 0; i < numWrite_; ++i) {
            j.add_process(
                sc_core::sc_spawn(
                    [&, i = i] {
                        while (true) {
                            auto aw = awFifos_[i]->read();
                            if (!waitAfterLastW_)
                                sc_core::wait(latencyWrite_);

                            Transaction transaction { (uint16_t)master.config.wData, master.config.axi3Compat };
                            transaction.reset(aw->addr.to_uint64(), aw->len, aw->size, aw->burst);

                            Beat beat;

                            uint8_t resp;
                            while (transaction.nextBeat(beat)) {
                                auto w = wFifos_[i]->read();
                                handler_->write(master.config, *aw, beat, *w, resp);
                                delete w;
                            }

                            if (waitAfterLastW_)
                                sc_core::wait(latencyWrite_);

                            auto b = new Packets::WriteResponse { master.makeB() };
                            b->id = aw->id;
                            b->resp = resp;
                            bFifos_[i]->write(b);

                            delete aw;
                        }
                    },
                    fmt::format("write_{}", i).c_str()
                )
            );
        }

        // B process
        j.add_process(
            sc_core::sc_spawn(
                [&] {
                    uint32_t i = 0;

                    while (true) {
                        auto b = bFifos_[i]->read();

                        master.sendB(*b);
                        if (log_)
                            fmt::print("Memory.b: [t = {}] sent: {}\n", sc_core::sc_time_stamp().to_string(), *b);

                        delete b;
                        i = (i + 1) % numWrite_;
                    }
                },
                "b"
            )
        );

        j.wait();
    }

    ~Memory() {
        for (auto x : arFifos_)
            delete x;

        for (auto x : rFifos_)
            delete x;

        for (auto x : awFifos_)
            delete x;

        for (auto x : wFifos_)
            delete x;

        for (auto x : bFifos_)
            delete x;
    }

private:
    std::shared_ptr<MemoryHandler> handler_;
    uint32_t numRead_, numWrite_;
    sc_core::sc_time latencyRead_, latencyWrite_;
    bool waitAfterLastW_, log_;

    bool isRunning_ = false;

    // clang-format off

    // TODO: replace the followings with a sc_fifo-friendly reference counted
    // smart pointer types.

    // Or, define a new queue type that is similar to sc_fifo but performs
    // in-place object construction for object storage.

    std::vector<
        sc_core::sc_fifo<Packets::ReadAddress *>*
    > arFifos_;

    std::vector<
        sc_core::sc_fifo<Packets::ReadData *>*
    > rFifos_;

    std::vector<
        sc_core::sc_fifo<Packets::WriteAddress *>*
    > awFifos_;

    std::vector<
        sc_core::sc_fifo<Packets::WriteData *>*
    > wFifos_;

    std::vector<
        sc_core::sc_fifo<Packets::WriteResponse *>*
    > bFifos_;

    // clang-format on
};

} // namespace chext_test::amba::axi4::full

#endif /* CHEXT_TEST_AMBA_AXI4_FULL_TRANSACTION_HPP_INCLUDED */
